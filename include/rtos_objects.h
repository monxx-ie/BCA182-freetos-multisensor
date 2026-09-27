#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"

/* SensorTask -> DisplayTask: latest SensorData */
extern QueueHandle_t sensorToDisplayQueue;

/* SensorTask -> AlarmTask: latest SensorData */
extern QueueHandle_t sensorToAlarmQueue;

bool RTOS_Objects_Create(void);

#endif