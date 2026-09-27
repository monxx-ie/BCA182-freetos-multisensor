#include "logic.h"

DisplayMode nextDisplayMode(DisplayMode mode)
{
    if (mode >= DISPLAY_MODE_COUNT) { return DISPLAY_TEMPERATURE; }
    return (DisplayMode)(((int)mode + 1) % (int)DISPLAY_MODE_COUNT);
}

DisplayMode previousDisplayMode(DisplayMode mode)
{
    if (mode >= DISPLAY_MODE_COUNT) { return DISPLAY_TEMPERATURE; }
    return (DisplayMode)(((int)mode + (int)DISPLAY_MODE_COUNT - 1) % (int)DISPLAY_MODE_COUNT);
}

const char *displayModeName(DisplayMode mode)
{
    switch (mode)
    {
        case DISPLAY_TEMPERATURE: return "Temperature";
        case DISPLAY_HUMIDITY:    return "Humidity";
        case DISPLAY_LIGHT:       return "Light";
        case DISPLAY_MOTION:      return "Motion";
        default:                  return "?";
    }
}