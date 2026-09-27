#ifndef LOG_H
#define LOG_H

#include <stdint.h>

void Log_Init(void);
void Log(const char *msg);
void Log_Tenths(int32_t v);     /* prints 254 as "25.4" */
void Log_Uint(uint32_t v);

/* Fault-safe output: writes USART1 registers directly,
   usable with interrupts disabled (asserts, faults) */
void Log_RawPuts(const char *s);
void Log_RawUint(uint32_t v);

#endif