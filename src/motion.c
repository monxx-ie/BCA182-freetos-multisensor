#include <stdbool.h>
#include "motion.h"
#include "rtos_objects.h"
#include "log.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#define PIR_PORT          GPIOA
#define PIR_PIN           GPIO_PIN_3
#define MOTION_PERIOD_MS  100U

void Motion_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin  = PIR_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLDOWN;   /* no motion = low */
    HAL_GPIO_Init(PIR_PORT, &g);
}

/* MotionTask: polls the PIR every 100 ms. Keeps running in both ACTIVE
   and INACTIVE states, because motion is what wakes the system up. */
void MotionTask(void *argument)
{
    (void)argument;
    bool lastLevel = false;

    for (;;)
    {
        bool level = (HAL_GPIO_ReadPin(PIR_PORT, PIR_PIN) == GPIO_PIN_SET);

        if (level)
        {
            /* EVENT_MOTION: "motion seen" signal for StateTask (it clears it).
               EVENT_PIR_LEVEL: current level, for SensorData.motionDetected. */
            (void)xEventGroupSetBits(systemEvents, EVENT_MOTION | EVENT_PIR_LEVEL);
        }
        else
        {
            (void)xEventGroupClearBits(systemEvents, EVENT_PIR_LEVEL);
        }

        if (level && !lastLevel) { Log("[MotionTask] Motion detected\r\n"); }
        if (!level && lastLevel) { Log("[MotionTask] Motion ended\r\n"); }
        lastLevel = level;

        vTaskDelay(pdMS_TO_TICKS(MOTION_PERIOD_MS));
    }
}