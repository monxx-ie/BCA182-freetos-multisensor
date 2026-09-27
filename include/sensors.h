#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>

/* One complete measurement, passed between tasks through queues */
typedef struct
{
    float temperature;      /* degrees C */
    float humidity;         /* % relative humidity */
    int   lightLevel;       /* relative light level, 0..100 % (not lux) */
    bool  motionDetected;   /* filled in once MotionTask exists (Part IX) */
    bool  dhtValid;         /* false if the latest DHT22 read failed */
} SensorData;

void Sensors_Init(void);
void SensorTask(void *argument);

#endif