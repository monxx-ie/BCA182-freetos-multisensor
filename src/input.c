#include "input.h"
#include "logic.h"
#include "rtos_objects.h"
#include "log.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define ENC_PORT         GPIOA
#define ENC_CLK_PIN      GPIO_PIN_4
#define ENC_DT_PIN       GPIO_PIN_5
#define INPUT_PERIOD_MS  50U

/* Steps counted by the ISR, consumed by InputTask.
   +1 = clockwise, -1 = counterclockwise. */
static volatile int32_t encoderSteps = 0;

void Input_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};

    /* CLK: interrupt on falling edge (one detent = one falling edge) */
    g.Pin  = ENC_CLK_PIN;
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENC_PORT, &g);

    /* DT: plain input, read inside the ISR to get the direction */
    g.Pin  = ENC_DT_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENC_PORT, &g);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(ENC_CLK_PIN);
}

/* Kept minimal: no FreeRTOS calls in the ISR. At the CLK falling edge,
   DT still high = clockwise, DT already low = counterclockwise. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ENC_CLK_PIN)
    {
        if (HAL_GPIO_ReadPin(ENC_PORT, ENC_DT_PIN) == GPIO_PIN_SET)
        {
            encoderSteps++;
        }
        else
        {
            encoderSteps--;
        }
    }
}

/* InputTask: every 50 ms, consumes encoder steps and publishes the
   selected page to DisplayTask through displayModeQueue. */
void InputTask(void *argument)
{
    (void)argument;
    DisplayMode mode = DISPLAY_TEMPERATURE;

    xQueueOverwrite(displayModeQueue, &mode);

    for (;;)
    {
        int32_t steps;

        /* Read-and-clear must be atomic, or a step arriving in between is lost */
        taskENTER_CRITICAL();
        steps = encoderSteps;
        encoderSteps = 0;
        taskEXIT_CRITICAL();

        if (steps != 0)
        {
            while (steps > 0) { mode = nextDisplayMode(mode);     steps--; }
            while (steps < 0) { mode = previousDisplayMode(mode); steps++; }

            xQueueOverwrite(displayModeQueue, &mode);

            Log("[InputTask] Page: ");
            Log(displayModeName(mode));
            Log("\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(INPUT_PERIOD_MS));
    }
}