#include "rtos_objects.h"
#include "sensors.h"

QueueHandle_t sensorToDisplayQueue = NULL;
QueueHandle_t sensorToAlarmQueue   = NULL;

bool RTOS_Objects_Create(void)
{
    sensorToDisplayQueue = xQueueCreate(1, sizeof(SensorData));
    sensorToAlarmQueue   = xQueueCreate(1, sizeof(SensorData));

    return (sensorToDisplayQueue != NULL) && (sensorToAlarmQueue != NULL);
}