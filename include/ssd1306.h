#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>
#include <stdbool.h>

/* Minimal SSD1306 128x64 I2C driver (I2C1: PB6 = SCL, PB7 = SDA).
   Drawing happens in a RAM frame buffer; SSD1306_Update() sends it. */
bool SSD1306_Init(void);
void SSD1306_Clear(void);
void SSD1306_DrawString(uint8_t x, uint8_t y, const char *s, uint8_t scale);
bool SSD1306_Update(void);
void SSD1306_SetPower(bool on);

#endif