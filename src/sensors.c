#include "sensors.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "dht22.h"
#include "ldr.h"
#include "rtos_objects.h"
#include "log.h"

#define SENSOR_PERIOD_MS  2000U

void Sensors_Init(void)
{
    DHT22_Init();
    LDR_Init();
}

/* SensorTask: reads DHT22 + LDR every 2 s and publishes the latest
   SensorData to its consumers. vTaskDelayUntil() keeps a fixed period
   measured from the previous wake time, so read time does not cause drift. */
void SensorTask(void *argument)
{
    (void)argument;
    SensorData data = {0};
    Dht22Reading r;
    uint16_t raw;
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        if (DHT22_Read(&r))
        {
            data.temperature = (float)r.temp_x10 / 10.0f;
            data.humidity    = (float)r.hum_x10 / 10.0f;
            data.dhtValid    = true;
        }
        else
        {
            data.dhtValid = false;      /* keep last values, mark them stale */
            Log("[SensorTask] DHT22 read failed\r\n");
        }

        if (LDR_ReadRaw(&raw))
        {
            data.lightLevel = LDR_RawToPercent(raw);
        }

        data.motionDetected = false;    /* PIR is added in Part IX */

        /* Length-1 queues + overwrite: each consumer always gets the
           NEWEST reading instead of a backlog of old ones. Two queues are
           used because receiving from a queue removes the item, so one
           consumer must not take data the other one still needs. */
        xQueueOverwrite(sensorToDisplayQueue, &data);
        xQueueOverwrite(sensorToAlarmQueue, &data);

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}