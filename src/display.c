#include <string.h>
#include "display.h"
#include "ssd1306.h"
#include "sensors.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Writes a value in tenths as text, e.g. 254 -> "25.4" */
static void FormatTenths(char *buf, int32_t v)
{
    char tmp[12];
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
}

static int32_t ToTenths(float v)
{
    return (int32_t)(v * 10.0f + ((v >= 0.0f) ? 0.5f : -0.5f));
}

static void Render(const SensorData *d)
{
    char value[16];

    if (d->dhtValid)
    {
        FormatTenths(value, ToTenths(d->temperature));
        strcat(value, " C");
    }
    else
    {
        strcpy(value, "--.- C");
    }

    SSD1306_Clear();
    SSD1306_DrawString(0, 0,  "ROOM MONITOR", 1);
    SSD1306_DrawString(0, 16, "Temperature", 1);
    SSD1306_DrawString(0, 32, value, 2);      /* 2x size for the reading */
    (void)SSD1306_Update();
}

void DisplayTask(void *argument)
{
    (void)argument;
    SensorData d;

    if (SSD1306_Init())
    {
        Log("[DisplayTask] OLED ready\r\n");
        SSD1306_Clear();
        SSD1306_DrawString(0, 0,  "ROOM MONITOR", 1);
        SSD1306_DrawString(0, 16, "Waiting...", 1);
        (void)SSD1306_Update();
    }
    else
    {
        Log("[DisplayTask] OLED init failed\r\n");
    }

    for (;;)
    {
        /* Blocked until SensorTask publishes new data: no CPU used while waiting */
        if (xQueueReceive(sensorToDisplayQueue, &d, portMAX_DELAY) == pdPASS)
        {
            Render(&d);
        }
    }
}