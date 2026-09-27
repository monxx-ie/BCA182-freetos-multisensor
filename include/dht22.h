#ifndef DHT22_H
#define DHT22_H

#include <stdint.h>
#include <stdbool.h>

/* Values in tenths: temp_x10 = 254 means 25.4 C, hum_x10 = 612 means 61.2 % */
typedef struct
{
    int16_t  temp_x10;
    uint16_t hum_x10;
} Dht22Reading;

void DHT22_Init(void);
bool DHT22_Read(Dht22Reading *out);

#endif