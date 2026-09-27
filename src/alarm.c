#include <stdbool.h>
#include "alarm.h"
#include "buzzer.h"
#include "logic.h"
#include "sensors.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#define ALARM_WAIT_MS  250U

void AlarmTask(void *argument)
{
    (void)argument;
    SensorData d;
    AlarmState state = ALARM_NORMAL;

    if (!Buzzer_Init())
    {
        Log("[AlarmTask] Buzzer init failed\r\n");
    }

    for (;;)
    {
        /* Blocked until a new reading arrives, or at most 250 ms so a change
           to INACTIVE silences the buzzer promptly */
        if (xQueueReceive(sensorToAlarmQueue, &d, pdMS_TO_TICKS(ALARM_WAIT_MS)) == pdPASS)
        {
            /* A failed DHT22 read is not treated as an alarm; keep the last state */
            if (d.dhtValid)
            {
                AlarmState newState = evaluateTemperature(d.temperature);   /* pure logic */

                if (newState != state)
                {
                    state = newState;
                    Log("[AlarmTask] Alarm state: ");
                    Log(alarmStateName(state));
                    Log("\r\n");

                    if (state != ALARM_NORMAL) { (void)xEventGroupSetBits(systemEvents, EVENT_ALARM); }
                    else                       { (void)xEventGroupClearBits(systemEvents, EVENT_ALARM); }
                }
            }
        }

        /* The alarm only sounds while the system is ACTIVE */
        bool active = (xEventGroupGetBits(systemEvents) & EVENT_ACTIVE) != 0;
        Buzzer_Set(active && state != ALARM_NORMAL);                        /* hardware */
    }
}