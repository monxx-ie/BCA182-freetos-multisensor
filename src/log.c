#include <string.h>
#include "log.h"
#include "rtos_objects.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static UART_HandleTypeDef huart1;

void Log_Init(void)
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

/* Before the scheduler starts there is only one thread of execution,
   so locking is skipped (and the mutex may not exist yet). */
static int MutexUsable(void)
{
    return (serialMutex != NULL) &&
           (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);
}

void Log_Begin(void)
{
    if (MutexUsable())
    {
        (void)xSemaphoreTakeRecursive(serialMutex, portMAX_DELAY);
    }
}

void Log_End(void)
{
    if (MutexUsable())
    {
        (void)xSemaphoreGiveRecursive(serialMutex);
    }
}

void Log(const char *msg)
{
    Log_Begin();
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, (uint16_t)strlen(msg), HAL_MAX_DELAY);
    Log_End();
}

void Log_Tenths(int32_t v)
{
    char tmp[12];
    char buf[12];
    int i = 0;
    int neg = (v < 0);
    if (neg) { v = -v; }

    tmp[i++] = (char)('0' + (v % 10)); v /= 10;
    tmp[i++] = '.';
    do { tmp[i++] = (char)('0' + (v % 10)); v /= 10; } while (v > 0);
    if (neg) { tmp[i++] = '-'; }

    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
    Log(buf);
}

void Log_Uint(uint32_t v)
{
    char tmp[12];
    char buf[12];
    int i = 0;
    do { tmp[i++] = (char)('0' + (v % 10)); v /= 10; } while (v > 0);

    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
    Log(buf);
}

static void RawPutc(char c)
{
    while ((USART1->SR & USART_SR_TXE) == 0) { }
    USART1->DR = (uint8_t)c;
}

void Log_RawPuts(const char *s)
{
    while (*s) { RawPutc(*s++); }
}

void Log_RawUint(uint32_t v)
{
    char num[12];
    int i = 10;
    num[11] = '\0';
    do { num[i--] = (char)('0' + v % 10); v /= 10; } while (v > 0 && i >= 0);
    Log_RawPuts(&num[i + 1]);
}