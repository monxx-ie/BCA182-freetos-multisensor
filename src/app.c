#include "app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "alarm.h"
#include "log.h"

#define TASK_STACK_WORDS       256

/* Priorities: higher number = more urgent */
#define INPUT_TASK_PRIORITY    3   /* user input must feel immediate */
#define SENSOR_TASK_PRIORITY   2   /* 2 s periodic sampling, tolerates small delays */
#define ALARM_TASK_PRIORITY    2   /* reacts to each new reading; 2 s cadence */
#define DISPLAY_TASK_PRIORITY  1   /* redraw can wait; slowest, least urgent */

void app_main(void)
{
    Log("System starting...\r\n");

    if (!RTOS_Objects_Create())
    {
        Log("ERROR: FreeRTOS object creation failed\r\n");
        for (;;) { }
    }

    BaseType_t ok = pdPASS;
    ok &= xTaskCreate(InputTask,   "InputTask",   TASK_STACK_WORDS, NULL, INPUT_TASK_PRIORITY,   NULL);
    ok &= xTaskCreate(SensorTask,  "SensorTask",  TASK_STACK_WORDS, NULL, SENSOR_TASK_PRIORITY,  NULL);
    ok &= xTaskCreate(AlarmTask,   "AlarmTask",   TASK_STACK_WORDS, NULL, ALARM_TASK_PRIORITY,   NULL);
    ok &= xTaskCreate(DisplayTask, "DisplayTask", TASK_STACK_WORDS, NULL, DISPLAY_TASK_PRIORITY, NULL);

    if (ok != pdPASS)
    {
        Log("ERROR: task creation failed\r\n");
        for (;;) { }
    }

    Log("FreeRTOS objects and tasks created\r\n");
    Log("Starting scheduler...\r\n");
    vTaskStartScheduler();

    Log("ERROR: scheduler failed to start\r\n");
    for (;;) { }
}