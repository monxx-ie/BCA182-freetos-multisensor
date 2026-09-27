#ifndef LOG_H
#define LOG_H

#include <stdint.h>

void Log_Init(void);

/* Thread-safe logging: every call is protected by serialMutex.
   Wrap a multi-part line in Log_Begin()/Log_End() so no other task
   can print in the middle of it. */
void Log(const char *msg);
void Log_Tenths(int32_t v);     /* prints 254 as "25.4" */
void Log_Uint(uint32_t v);
void Log_Begin(void);
void Log_End(void);

/* Fault-safe output: writes USART1 registers directly, no mutex,
   usable with interrupts disabled (asserts, faults) */
void Log_RawPuts(const char *s);
void Log_RawUint(uint32_t v);

#endif