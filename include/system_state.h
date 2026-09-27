#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

/* StateTask centralizes the ACTIVE/INACTIVE state machine and publishes
   the current state as EVENT_ACTIVE in the systemEvents event group. */
void StateTask(void *argument);

#endif