#include "stm32f1xx_hal.h"
#include "log.h"
#include "sensors.h"
#include "app.h"

void Error_Handler(void)
{
    __disable_irq();
    for (;;) { }
}

/* PC13 on-board LED: used as a visual fault indicator */
static void StatusLed_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin   = GPIO_PIN_13;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &g);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* off */
}

/* main(): hardware initialization only, then hand over to app_main() */
int main(void)
{
    SCB->VTOR = FLASH_BASE;

    HAL_Init();
    StatusLed_Init();
    Log_Init();
    Sensors_Init();

    Log("BCA182 FreeRTOS Multisensor\r\n");

    app_main();   /* creates RTOS objects + tasks, starts the scheduler */

    for (;;) { }
}