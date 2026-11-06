#ifndef INC_INPUT_PROCESSING_H_
#define INC_INPUT_PROCESSING_H_

#include "input_reading.h"

void input_processing(void);

/* Returns 1 once per new press and on each auto-increment tick when held, then clears event. */
uint8_t input_get_press_event(uint8_t index);

/* Returns 1 once when the button is held for >= 1 second, then clears event. */
uint8_t input_get_long_press_event(uint8_t index);

#endif