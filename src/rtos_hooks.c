#include "FreeRTOS.h"
#include "task.h"
#include "stm32f1xx_hal.h"
#include "log.h"

/* Called by FreeRTOS when a configASSERT check fails */
void vAssertCalled(const char *file, int line)
{
    __disable_irq();
    GPIOC->BSRR = (uint32_t)GPIO_PIN_13 << 16;   /* PC13 LED on */
    Log_RawPuts("\r\nASSERT FAILED: ");
    Log_RawPuts(file);
    Log_RawPuts(" line ");
    Log_RawUint((uint32_t)line);
    Log_RawPuts("\r\n");
    for (;;) { }
}

/* Called by FreeRTOS if a task overflows its stack */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    __disable_irq();
    Log_RawPuts("\r\nSTACK OVERFLOW in task: ");
    Log_RawPuts(pcTaskName);
    Log_RawPuts("\r\n");
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