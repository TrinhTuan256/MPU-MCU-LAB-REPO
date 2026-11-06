#ifndef LED_DISPLAY_H
#define LED_DISPLAY_H

#include "board_pins.h"
#include <stdint.h>

/* Traffic-light outputs */
typedef enum
{
    LED_EW_RED = 0,
    LED_EW_AMBER,
    LED_EW_GREEN,

    LED_NS_RED,
    LED_NS_AMBER,
    LED_NS_GREEN,

    LED_COUNT
} LedID;

#define SEVEN_SEG_DIGIT_COUNT 4U

/* Logical output buffers: 0 = off, 1 = on for traffic LEDs. */
extern volatile uint8_t led_output_buffer[LED_COUNT];

/* Each element stores a digit from 0 to 9. */
extern volatile uint8_t seven_seg_buffer[SEVEN_SEG_DIGIT_COUNT];

void led_display_init(void);

void led_display_set_led(LedID id, uint8_t on);
void led_display_toggle_led(LedID id);
void led_display_clear_all_leds(void);

void led_display_set_digit(uint8_t position, uint8_t value);
void led_display_set_2digit_value(uint8_t left_position, uint8_t value);

/* Call periodically from the main loop. */
void led_display_task(void);

#endif