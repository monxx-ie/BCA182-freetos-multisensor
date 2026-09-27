#ifndef BUZZER_H
#define BUZZER_H

#include <stdbool.h>

/* Passive buzzer on PA2 driven by TIM2 channel 3 PWM (1 kHz tone) */
bool Buzzer_Init(void);
void Buzzer_Set(bool on);

#endif