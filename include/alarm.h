#ifndef ALARM_H
#define ALARM_H

/* AlarmTask owns the buzzer: evaluates each new reading against the
   18..30 C range and switches the buzzer on/off. */
void AlarmTask(void *argument);

#endif