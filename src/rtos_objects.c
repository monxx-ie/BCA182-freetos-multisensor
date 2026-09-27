#include "rtos_objects.h"
#include "sensors.h"
#include "logic.h"

QueueHandle_t sensorToDisplayQueue = NULL;
QueueHandle_t sensorToAlarmQueue   = NULL;
QueueHandle_t displayModeQueue     = NULL;

bool RTOS_Objects_Create(void)
{
    sensorToDisplayQueue = xQueueCreate(1, sizeof(SensorData));
    sensorToAlarmQueue   = xQueueCreate(1, sizeof(SensorData));
    displayModeQueue     = xQueueCreate(1, sizeof(DisplayMode));

    return (sensorToDisplayQueue != NULL) &&
           (sensorToAlarmQueue   != NULL) &&
           (displayModeQueue     != NULL);
}