#ifndef INPUT_H
#define INPUT_H

/* Rotary encoder: CLK = PA4 (EXTI4 interrupt), DT = PA5 */
void Input_Init(void);
void InputTask(void *argument);

#endif