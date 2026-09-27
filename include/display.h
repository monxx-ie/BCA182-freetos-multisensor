#ifndef DISPLAY_H
#define DISPLAY_H

/* DisplayTask is the ONLY task that accesses the OLED (single owner),
   so no other task can interleave I2C traffic or corrupt the screen. */
void DisplayTask(void *argument);

#endif