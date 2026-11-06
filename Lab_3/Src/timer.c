
#include "timer.h"

typedef struct
{
    volatile uint32_t counter;
    //volatile uint32_t duration_ticks;
    volatile uint8_t flag;
    volatile uint8_t active;
} SoftwareTimer;

static SoftwareTimer timers[TIMER_TOTAL];

void timer_set(TimerID id, uint32_t duration_ms)
{
    if (id >= TIMER_TOTAL)
        return;

    // Convert milliseconds to 10 ms ticks, rounding up.
    uint32_t ticks = (duration_ms + TIMER_TICK_MS - 1U) / TIMER_TICK_MS;

    if (ticks == 0U)
        ticks = 1U;

    // Prevent the interrupt from accessing partially updated fields.
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    timers[id].counter = ticks;
    timers[id].flag = 0U;
    timers[id].active = 1U;

    __set_PRIMASK(primask);
}

uint8_t timer_read_and_clear(TimerID id)
{
    if (id >= TIMER_TOTAL)
        return 0U;

    // Read and clear the flag atomically relative to the ISR.
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    uint8_t result = timers[id].flag;
    timers[id].flag = 0U;

    __set_PRIMASK(primask);

    return result;
}

// Called once per TIM2 interrupt: every 10 ms.
void timer_tick(void)
{
    for (uint32_t i = 0U; i < TIMER_TOTAL; i++)
    {
        if (!timers[i].active) continue;

        if (timers[i].counter > 0U)
        {    
            --timers[i].counter;

            if (timers[i].counter == 0U) timers[i].flag = 1U;
        };
    }
}

// disable timer
void timer_disable(TimerID id)
{
    if (id >= TIMER_TOTAL) return;

    // Prevent the interrupt from accessing partially updated fields.
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    timers[id].active = 0U;
    timers[id].counter = 0U;
    timers[id].flag = 0U;

    __set_PRIMASK(primask);
}
