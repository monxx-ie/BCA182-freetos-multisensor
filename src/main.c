#include <string.h>

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

#include "dht22.h"

static UART_HandleTypeDef huart1;

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

static void LED_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

static void UART1_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);   /* pins are set up in stm32f1xx_hal_msp.c */
}

static void Log(const char *msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), HAL_MAX_DELAY);
}

/* Formats a value in tenths (e.g. 254 -> "25.4") into buf */
static void FormatTenths(char *buf, int32_t v)
{
    char tmp[12];
    int i = 0;
    int neg = (v < 0);
    if (neg) { v = -v; }

    tmp[i++] = (char)('0' + (v % 10)); v /= 10;
    tmp[i++] = '.';
    do { tmp[i++] = (char)('0' + (v % 10)); v /= 10; } while (v > 0);
    if (neg) { tmp[i++] = '-'; }

    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
}

/* ---------- Raw UART output for fault reporting ---------- */
static void RawPutc(char c)
{
    while ((USART1->SR & USART_SR_TXE) == 0) { }
    USART1->DR = (uint8_t)c;
}

static void RawPuts(const char *s)
{
    while (*s) { RawPutc(*s++); }
}

static void RawPutNum(uint32_t n)
{
    char num[12];
    int i = 10;
    num[11] = '\0';
    do { num[i--] = (char)('0' + n % 10); n /= 10; } while (n > 0 && i >= 0);
    RawPuts(&num[i + 1]);
}

/* Called by FreeRTOS when a configASSERT check fails */
void vAssertCalled(const char *file, int line)
{
    __disable_irq();
    GPIOC->BSRR = (uint32_t)GPIO_PIN_13 << 16;   /* LED on */
    RawPuts("\r\nASSERT FAILED: ");
    RawPuts(file);
    RawPuts(" line ");
    RawPutNum((uint32_t)line);
    RawPuts("\r\n");
    for (;;) { }
}

/* Called by FreeRTOS if a task overflows its stack */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    __disable_irq();
    RawPuts("\r\nSTACK OVERFLOW in task: ");
    RawPuts(pcTaskName);
    RawPuts("\r\n");
    for (;;) { }
}

/* HAL tick before the scheduler starts: TIM4 is HAL's timebase.
   After the scheduler starts, the Wokwi port's TIM3 tick advances HAL time. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4 &&
        xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        HAL_IncTick();
    }
}

/* Provided by the Wokwi compatibility port (lib/FreeRTOS, credit: Ni-ear) */
extern BaseType_t xPortConsumeTickYield(void);

/* Idle hook: sleep until the next interrupt, then switch tasks only if a
   tick actually woke a higher-priority task. */
void vApplicationIdleHook(void)
{
    __WFI();
    if (xPortConsumeTickYield() != pdFALSE)
    {
        taskYIELD();
    }
}

#define TASK_A_PERIOD_MS   1000
#define DHT_PERIOD_MS      2000
#define TASK_A_PRIORITY    1
#define DHT_TASK_PRIORITY  2
#define TASK_STACK_WORDS   256

static void TaskA(void *argument)
{
    (void)argument;

    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        Log("Task A running\r\n");
        vTaskDelay(pdMS_TO_TICKS(TASK_A_PERIOD_MS));
    }
}

/* Step 20: verify the DHT22 over serial before building SensorTask */
static void DhtTestTask(void *argument)
{
    (void)argument;
    Dht22Reading r;
    char num[12];

    for (;;)
    {
        if (DHT22_Read(&r))
        {
            Log("Temperature: ");
            FormatTenths(num, r.temp_x10);
            Log(num);
            Log(" C\r\n");

            Log("Humidity: ");
            FormatTenths(num, (int32_t)r.hum_x10);
            Log(num);
            Log(" %\r\n");
        }
        else
        {
            Log("DHT22 read failed\r\n");
        }
        vTaskDelay(pdMS_TO_TICKS(DHT_PERIOD_MS));
    }
}

int main(void)
{
    SCB->VTOR = FLASH_BASE;

    HAL_Init();
    LED_Init();
    UART1_Init();
    DHT22_Init();

    Log("BCA182 FreeRTOS Multisensor\r\n");
    Log("System starting...\r\n");

    BaseType_t okA = xTaskCreate(TaskA, "TaskA", TASK_STACK_WORDS, NULL, TASK_A_PRIORITY, NULL);
    BaseType_t okD = xTaskCreate(DhtTestTask, "DhtTest", TASK_STACK_WORDS, NULL, DHT_TASK_PRIORITY, NULL);

    if (okA != pdPASS || okD != pdPASS)
    {
        Log("ERROR: task creation failed\r\n");
        while (1)
        {
        }
    }

    Log("Starting scheduler...\r\n");
    vTaskStartScheduler();

    Log("ERROR: scheduler failed to start\r\n");
    while (1)
    {
    }
}