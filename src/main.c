#include <string.h>

#include "stm32f1xx_hal.h"

#include "FreeRTOS.h"
#include "task.h"

static UART_HandleTypeDef huart1;

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

static void LED_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}

static void UART1_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);   /* pins are set up in stm32f1xx_hal_msp.c */
}

static void Log(const char *msg)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), HAL_MAX_DELAY);
}

#define TASK_A_PERIOD_MS   1000
#define TASK_B_PERIOD_MS   1000
#define TASK_A_PRIORITY    1
#define TASK_B_PRIORITY    2
#define TASK_STACK_WORDS   256

static void TaskA(void *argument)
{
    (void)argument;

    for (;;)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        Log("Task A running\r\n");
        vTaskDelay(pdMS_TO_TICKS(TASK_A_PERIOD_MS));
    }
}

static void TaskB(void *argument)
{
    (void)argument;

    for (;;)
    {
        Log("Task B running\r\n");
        vTaskDelay(pdMS_TO_TICKS(TASK_B_PERIOD_MS));
    }
}

int main(void)
{
    SCB->VTOR = FLASH_BASE;

    HAL_Init();       /* HAL now uses TIM4 for its tick, FreeRTOS gets SysTick */
    LED_Init();
    UART1_Init();

    Log("BCA182 FreeRTOS Multisensor\r\n");
    Log("System starting...\r\n");

    BaseType_t okA = xTaskCreate(TaskA, "TaskA", TASK_STACK_WORDS, NULL, TASK_A_PRIORITY, NULL);
    BaseType_t okB = xTaskCreate(TaskB, "TaskB", TASK_STACK_WORDS, NULL, TASK_B_PRIORITY, NULL);

    if (okA != pdPASS || okB != pdPASS)
    {
        Log("ERROR: task creation failed\r\n");
        while (1)
        {
        }
    }

    Log("Starting scheduler...\r\n");
    vTaskStartScheduler();

    Log("ERROR: scheduler failed to start\r\n");
    while (1)
    {
    }
}