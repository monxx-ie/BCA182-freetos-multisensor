#include "rtos_objects.h"
#include "sensors.h"
#include "logic.h"

QueueHandle_t sensorToDisplayQueue = NULL;
QueueHandle_t sensorToAlarmQueue   = NULL;
QueueHandle_t displayModeQueue     = NULL;
EventGroupHandle_t systemEvents    = NULL;
SemaphoreHandle_t serialMutex      = NULL;

bool RTOS_Objects_Create(void)
{
    sensorToDisplayQueue = xQueueCreate(1, sizeof(SensorData));
    sensorToAlarmQueue   = xQueueCreate(1, sizeof(SensorData));
    displayModeQueue     = xQueueCreate(1, sizeof(DisplayMode));
    systemEvents         = xEventGroupCreate();
    serialMutex          = xSemaphoreCreateRecursiveMutex();

    if (sensorToDisplayQueue == NULL || sensorToAlarmQueue == NULL ||
        displayModeQueue == NULL || systemEvents == NULL || serialMutex == NULL)
    {
        return false;
    }

    /* The system starts ACTIVE (the 15 s inactivity timer starts at boot) */
    (void)xEventGroupSetBits(systemEvents, EVENT_ACTIVE);
    return true;
}