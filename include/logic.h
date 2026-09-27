#ifndef LOGIC_H
#define LOGIC_H

#include <stdbool.h>
#include <stdint.h>

/* Hardware-independent decision logic (unit-testable on the PC) */

/* ---------- Display navigation ---------- */
typedef enum
{
    DISPLAY_TEMPERATURE = 0,
    DISPLAY_HUMIDITY,
    DISPLAY_LIGHT,
    DISPLAY_MOTION,
    DISPLAY_MODE_COUNT
} DisplayMode;

/* Clockwise:        Temperature -> Humidity -> Light -> Motion -> Temperature */
DisplayMode nextDisplayMode(DisplayMode mode);

/* Counterclockwise: reverse order, including wraparound */
DisplayMode previousDisplayMode(DisplayMode mode);

const char *displayModeName(DisplayMode mode);

/* ---------- Temperature alarm ---------- */
#define LOW_TEMPERATURE_LIMIT   18.0f   /* degrees C */
#define HIGH_TEMPERATURE_LIMIT  30.0f   /* degrees C */

typedef enum
{
    ALARM_NORMAL = 0,
    ALARM_LOW_TEMPERATURE,
    ALARM_HIGH_TEMPERATURE
} AlarmState;

/* Below 18 C -> LOW, above 30 C -> HIGH, 18..30 inclusive -> NORMAL */
AlarmState evaluateTemperature(float temperature);

const char *alarmStateName(AlarmState state);

/* ---------- System activity state machine ---------- */
#define INACTIVITY_TIMEOUT_MS  15000U   /* short timeout for laboratory testing */

typedef enum
{
    SYSTEM_ACTIVE = 0,
    SYSTEM_INACTIVE
} SystemState;

/* ACTIVE   + no motion for >= timeout -> INACTIVE
   INACTIVE + motion                   -> ACTIVE
   otherwise                           -> unchanged */
SystemState evaluateSystemState(SystemState current,
                                bool motionDetected,
                                uint32_t msSinceLastMotion,
                                uint32_t timeoutMs);

const char *systemStateName(SystemState state);

#endif