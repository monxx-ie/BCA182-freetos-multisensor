#ifndef LOGIC_H
#define LOGIC_H

/* Hardware-independent decision logic (unit-testable on the PC) */

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

#endif