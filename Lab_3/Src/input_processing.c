#include "input_processing.h"

/* Timing parameters (assuming input_processing is called every 10 ms) */
#define LONG_PRESS_THRESHOLD_TICKS  100U /* 100 * 10 ms = 1000 ms to trigger long press */
#define AUTO_REPEAT_CYCLE_TICKS     20U  /* 20 * 10 ms = 200 ms interval between auto-increments */

static uint8_t previousButtonState[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

/* One-shot press events waiting for the application to consume them. */
static uint8_t buttonPressEvent[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

/* One-shot long press event flags */
static uint8_t buttonLongPressEvent[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

/* Counters to track duration of hold and repeat intervals */
static uint16_t buttonHoldCounter[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

static uint16_t buttonRepeatCounter[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

// Process debounced button inputs, generate press events and auto-increment on long hold
void input_processing(void)
{
    for (uint8_t i = 0U; i < N0_OF_BUTTONS; i++)
    {
        uint8_t currentState = is_button_pressed(i);

        if (currentState == 1U)
        {
            if (previousButtonState[i] == 0U)
            {
                /* Rising edge: Initial button press */
                buttonPressEvent[i] = 1U;
                buttonHoldCounter[i] = 0U;
                buttonRepeatCounter[i] = 0U;
            }
            else
            {
                /* Button is being held */
                if (buttonHoldCounter[i] < LONG_PRESS_THRESHOLD_TICKS)
                {
                    buttonHoldCounter[i]++;
                    if (buttonHoldCounter[i] >= LONG_PRESS_THRESHOLD_TICKS)
                    {
                        /* 1 second reached: trigger long press event */
                        buttonLongPressEvent[i] = 1U;
                        /* Generate immediate auto-increment tick */
                        buttonPressEvent[i] = 1U;
                        buttonRepeatCounter[i] = 0U;
                    }
                }
                else
                {
                    /* Already in long press mode: periodic auto-repeat */
                    buttonRepeatCounter[i]++;
                    if (buttonRepeatCounter[i] >= AUTO_REPEAT_CYCLE_TICKS)
                    {
                        buttonRepeatCounter[i] = 0U;
                        buttonPressEvent[i] = 1U; /* Auto-increment tick */
                    }
                }
            }
        }
        else
        {
            /* Button released */
            buttonHoldCounter[i] = 0U;
            buttonRepeatCounter[i] = 0U;
        }

        previousButtonState[i] = currentState;
    }
}

// Return and clear the press event (fires on single click and periodic auto-increment ticks)
uint8_t input_get_press_event(uint8_t index)
{
    if (index >= N0_OF_BUTTONS)
        return 0U;

    uint8_t event = buttonPressEvent[index];
    buttonPressEvent[index] = 0U;

    return event;
}

// Return and clear the long-press event (fires once when held >= 1s)
uint8_t input_get_long_press_event(uint8_t index)
{
    if (index >= N0_OF_BUTTONS)
        return 0U;

    uint8_t event = buttonLongPressEvent[index];
    buttonLongPressEvent[index] = 0U;

    return event;
}