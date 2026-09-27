#ifndef LDR_H
#define LDR_H

#include <stdint.h>
#include <stdbool.h>

void    LDR_Init(void);
bool    LDR_ReadRaw(uint16_t *raw);          /* 12-bit ADC value, 0..4095 */
uint8_t LDR_RawToPercent(uint16_t raw);      /* relative light level, 0..100 % */

#endif