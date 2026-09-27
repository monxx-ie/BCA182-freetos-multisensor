#include "dht22.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

/* DHT22 data pin: PA1 (single-wire protocol) */
#define DHT_PORT  GPIOA
#define DHT_PIN   GPIO_PIN_1

static uint32_t cyclesPerUs = 8U;

/* DWT cycle counter gives microsecond timing without using a timer
   (Wokwi supports DWT on the Blue Pill) */
static void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    cyclesPerUs = SystemCoreClock / 1000000U;
    if (cyclesPerUs == 0U) { cyclesPerUs = 1U; }
}

static void DelayUs(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * cyclesPerUs;
    uint32_t guard = us * 20U;          /* fallback if the cycle counter stalls */

    for (;;)
    {
        uint32_t elapsed = DWT->CYCCNT - start;   /* wrap-safe unsigned difference */
                /* cppcheck-suppress unsignedLessThanZero ; false positive: wrap-safe elapsed-time check, no sign comparison */
        if (elapsed >= ticks)  { break; }
        if (guard-- == 0U)     { break; }
    }
}

/* MCU drives the line (used only for the start signal) */
static void PinAsOutput(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin   = DHT_PIN;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT_PORT, &g);
}

/* MCU releases the line; the pull-up holds it high so the sensor can answer.
   On STM32F1 the internal pull-up only works in input mode. */
static void PinAsInput(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin  = DHT_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT_PORT, &g);
}

/* Wait until the pin reaches 'level'. Returns elapsed microseconds,
   or -1 on timeout. Bounded by both time and an iteration limit,
   so it can never loop forever. */
static int WaitLevel(uint32_t level, uint32_t timeoutUs)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t limit = timeoutUs * cyclesPerUs;
    uint32_t guard = timeoutUs * 20U;   /* fallback if the cycle counter stalls */

    for (;;)
    {
        uint32_t elapsed = DWT->CYCCNT - start;   /* wrap-safe unsigned difference */
        uint32_t pin = ((DHT_PORT->IDR & DHT_PIN) != 0U) ? 1U : 0U;

        if (pin == level)     { return (int)(elapsed / cyclesPerUs); }
                /* cppcheck-suppress unsignedLessThanZero ; false positive: wrap-safe elapsed-time check, no sign comparison */
        if (elapsed > limit)  { return -1; }
        if (guard-- == 0U)    { return -1; }
    }
}

void DHT22_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    PinAsInput();          /* idle: released, pulled high */
    DWT_Init();
}

bool DHT22_Read(Dht22Reading *out)
{
    uint8_t data[5] = {0};
    bool ok = false;

    /* Start signal: drive the line low for >1 ms */
    PinAsOutput();
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_RESET);
    DelayUs(1200);

    /* The reply is timing-critical (~5 ms), so no interrupt or task switch
       is allowed here (the port masks interrupts with PRIMASK) */
    taskENTER_CRITICAL();

    /* Release the line and listen */
    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, GPIO_PIN_SET);
    PinAsInput();

    /* Sensor response: low ~80 us, high ~80 us, then the first bit starts */
    if (WaitLevel(0U, 200U) < 0) goto done;
    if (WaitLevel(1U, 200U) < 0) goto done;
    if (WaitLevel(0U, 200U) < 0) goto done;

    /* 40 bits: each bit = ~50 us low, then high for ~26 us (0) or ~70 us (1) */
    for (int i = 0; i < 40; i++)
    {
        if (WaitLevel(1U, 100U) < 0) goto done;
        int highUs = WaitLevel(0U, 120U);
        if (highUs < 0) goto done;

        data[i / 8] = (uint8_t)(data[i / 8] << 1);
        if (highUs > 40) { data[i / 8] |= 1U; }
    }

    /* Checksum = low byte of the sum of the first 4 bytes */
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) == data[4])
    {
        int16_t t = (int16_t)(((data[2] & 0x7FU) << 8) | data[3]);
        if (data[2] & 0x80U) { t = (int16_t)-t; }

        out->hum_x10  = (uint16_t)((data[0] << 8) | data[1]);
        out->temp_x10 = t;
        ok = true;
    }

done:
    taskEXIT_CRITICAL();
    return ok;
}