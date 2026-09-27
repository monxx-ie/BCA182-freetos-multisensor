#include "alarm.h"
#include "buzzer.h"
#include "logic.h"
#include "sensors.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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
        /* Blocked until SensorTask publishes a new reading */
        if (xQueueReceive(sensorToAlarmQueue, &d, portMAX_DELAY) != pdPASS)
        {
            continue;
        }

        /* A failed DHT22 read is not treated as an alarm; keep the last state */
        if (!d.dhtValid)
        {
            continue;
        }

        AlarmState newState = evaluateTemperature(d.temperature);   /* pure logic */

        if (newState != state)
        {
            state = newState;
            Log("[AlarmTask] Alarm state: ");
            Log(alarmStateName(state));
            Log("\r\n");
        }

        Buzzer_Set(state != ALARM_NORMAL);                          /* hardware */
    }
}