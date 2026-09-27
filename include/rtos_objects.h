#ifndef RTOS_OBJECTS_H
#define RTOS_OBJECTS_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
#include "semphr.h"

/* ---------- Queues ---------- */

/* SensorTask -> DisplayTask: latest SensorData */
extern QueueHandle_t sensorToDisplayQueue;

/* SensorTask -> AlarmTask: latest SensorData */
extern QueueHandle_t sensorToAlarmQueue;

/* InputTask -> DisplayTask: currently selected DisplayMode */
extern QueueHandle_t displayModeQueue;

/* ---------- Event group: systemEvents ----------
   EVENT_ACTIVE    set/cleared by StateTask;  read by Display/Input/Alarm tasks
   EVENT_MOTION    set by MotionTask when motion is seen;
                   consumed and cleared by StateTask
   EVENT_ALARM     set/cleared by AlarmTask;  read by DisplayTask
   EVENT_PIR_LEVEL set/cleared by MotionTask (current PIR level);
                   read by SensorTask                                  */
#define EVENT_ACTIVE     ((EventBits_t)(1UL << 0))
#define EVENT_MOTION     ((EventBits_t)(1UL << 1))
#define EVENT_ALARM      ((EventBits_t)(1UL << 2))
#define EVENT_PIR_LEVEL  ((EventBits_t)(1UL << 3))

extern EventGroupHandle_t systemEvents;

/* ---------- Mutex ----------
   serialMutex: protects USART1 diagnostic output shared by all tasks.
   Recursive so Log_Begin()/Log_End() can wrap several Log() calls. */
extern SemaphoreHandle_t serialMutex;

bool RTOS_Objects_Create(void);

#endif