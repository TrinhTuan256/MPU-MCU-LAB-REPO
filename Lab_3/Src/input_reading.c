#include "input_reading.h"
#include "board_pins.h"

static const uint16_t button_pins[N0_OF_BUTTONS] =
{
    BUTTON_1_Pin,   /* PA1 */
    BUTTON_2_Pin,   /* PA2 */
    BUTTON_3_Pin    /* PA3 */
};

/* Debounced button states: 0 = released, 1 = pressed. */
static volatile uint8_t buttonBuffer[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

/* Previous raw samples used for debouncing. */
static uint8_t previousSample[N0_OF_BUTTONS] =
{
    0U, 0U, 0U
};

/* 1-second long-press detection (100 samples * 10ms = 1000ms) */
#define DURATION_FOR_1S 100U
static uint16_t counterForButtonPress1s[N0_OF_BUTTONS] = {0U, 0U, 0U};
static uint8_t flagForButtonPress1s[N0_OF_BUTTONS] = {0U, 0U, 0U};

// reading button state, simple debounce by checking previous sample
// setting the debounce interval using the timer
void button_reading(void)
{
    for (uint8_t i = 0U; i < N0_OF_BUTTONS; i++)
    {
        /*
         * Pull-down configuration:
         * GPIO_PIN_SET   = pressed
         * GPIO_PIN_RESET = released
         */
        uint8_t rawSample = (HAL_GPIO_ReadPin(BUTTON_GPIO_Port, button_pins[i]) == GPIO_PIN_SET) ? 1U : 0U;


        /* Accept a state after two consecutive matching samples. */
        if (rawSample == previousSample[i])
        {
            buttonBuffer[i] = rawSample;

            if (buttonBuffer[i] == 1U)
            {
                if (counterForButtonPress1s[i] < DURATION_FOR_1S)
                {
                    counterForButtonPress1s[i]++;
                }
                else
                {
                    flagForButtonPress1s[i] = 1U;
                }
            }
            else
            {
                counterForButtonPress1s[i] = 0U;
                flagForButtonPress1s[i] = 0U;
            }
        }


        previousSample[i] = rawSample;
    }
}


// just return the button state
uint8_t is_button_pressed(uint8_t index)
{
    if (index >= N0_OF_BUTTONS)
        return 0U;

    return buttonBuffer[index];
}

uint8_t is_button_pressed_1s(uint8_t index)
{
    if (index >= N0_OF_BUTTONS)
        return 0U;

    return flagForButtonPress1s[index];
}