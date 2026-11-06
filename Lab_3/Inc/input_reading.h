#ifndef INC_INPUT_READING_H_
#define INC_INPUT_READING_H_

#include "main.h"
#include <stdint.h>

#define N0_OF_BUTTONS 3U

typedef enum
{
    BUTTON_1_INDEX = 0U,
    BUTTON_2_INDEX,
    BUTTON_3_INDEX
} ButtonIndex;

void button_reading(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t is_button_pressed_1s(uint8_t index);

#endif