#include <stdbool.h>
#include "system_state.h"
#include "logic.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#define STATE_CHECK_MS  250U

void StateTask(void *argument)
{
    (void)argument;
    SystemState state = SYSTEM_ACTIVE;
    TickType_t lastMotionTick = xTaskGetTickCount();

    Log("[StateTask] System ACTIVE\r\n");

    for (;;)
    {
        /* Blocked until MotionTask signals motion, or at most 250 ms so the
           inactivity timeout is still checked. pdTRUE = clear EVENT_MOTION
           on exit, so each motion signal is consumed exactly once. */
        EventBits_t bits = xEventGroupWaitBits(systemEvents, EVENT_MOTION,
                                               pdTRUE, pdFALSE,
                                               pdMS_TO_TICKS(STATE_CHECK_MS));

        bool motion = (bits & EVENT_MOTION) != 0;
        TickType_t now = xTaskGetTickCount();

        if (motion)
        {
            lastMotionTick = now;
        }

        uint32_t msSinceMotion = (uint32_t)(now - lastMotionTick) * portTICK_PERIOD_MS;

        SystemState newState = evaluateSystemState(state, motion, msSinceMotion,
                                                   INACTIVITY_TIMEOUT_MS);   /* pure logic */

        if (newState != state)
        {
            state = newState;

            if (state == SYSTEM_ACTIVE)
            {
                (void)xEventGroupSetBits(systemEvents, EVENT_ACTIVE);
            }
            else
            {
                (void)xEventGroupClearBits(systemEvents, EVENT_ACTIVE);
            }

            Log_Begin();
            Log("[StateTask] System ");
            Log(systemStateName(state));
            Log("\r\n");
            Log_End();
        }
    }
}