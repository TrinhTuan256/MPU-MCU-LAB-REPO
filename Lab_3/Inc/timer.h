#ifndef TIMER_H
#define TIMER_H

#include "main.h"
#include <stdint.h>

#define TIMER_TICK_MS       10U

// Add more timer IDs as needed.
typedef enum
{
    TIMER_DISPLAY = 0,
    TIMER_BUTTON,
    TIMER_BLINK,
    TIMER_COUNTDOWN,
    // add more timer IDs here
    TIMER_TOTAL
} TimerID;

void timer_set(TimerID id, uint32_t duration_ms);
uint8_t timer_read_and_clear(TimerID id);
void timer_tick(void);
void timer_disable(TimerID id);

#endif
