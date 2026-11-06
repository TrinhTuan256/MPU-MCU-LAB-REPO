#include "led_display.h"

/* ---------- Output polarity ---------- */

#define LED_ACTIVE_LOW     0U
#define SEG_ACTIVE_LOW     1U
#define DIGIT_ACTIVE_LOW   1U

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
} GpioPin;

/* ---------- Output buffers ---------- */

volatile uint8_t led_output_buffer[LED_COUNT] = {0U};

volatile uint8_t seven_seg_buffer[SEVEN_SEG_DIGIT_COUNT] = {0U};

/* ---------- Traffic LED GPIO mapping ---------- */

static const GpioPin led_pins[LED_COUNT] =
{
    {LED_EW_GPIO_Port, LED_EW_RED_Pin},
    {LED_EW_GPIO_Port, LED_EW_AMBER_Pin},
    {LED_EW_GPIO_Port, LED_EW_GREEN_Pin},

    {LED_NS_GPIO_Port, LED_NS_RED_Pin},
    {LED_NS_GPIO_Port, LED_NS_AMBER_Pin},
    {LED_NS_GPIO_Port, LED_NS_GREEN_Pin}
};

/* ---------- Seven-segment GPIO mapping ---------- */
/* Bit order: bit 0 = A, bit 1 = B, ... bit 6 = G. */

static const GpioPin segment_pins[7] =
{
    {SEG_GPIO_Port, SEG_A_Pin},
    {SEG_GPIO_Port, SEG_B_Pin},
    {SEG_GPIO_Port, SEG_C_Pin},
    {SEG_GPIO_Port, SEG_D_Pin},
    {SEG_GPIO_Port, SEG_E_Pin},
    {SEG_GPIO_Port, SEG_F_Pin},
    {SEG_GPIO_Port, SEG_G_Pin}
};

/* ---------- Digit-select GPIO mapping ---------- */

static const GpioPin digit_pins[SEVEN_SEG_DIGIT_COUNT] =
{
    {SEG_SWITCH_GPIO_Port, SEG_SWITCH_1_Pin},
    {SEG_SWITCH_GPIO_Port, SEG_SWITCH_2_Pin},
    {SEG_SWITCH_GPIO_Port, SEG_SWITCH_3_Pin},
    {SEG_SWITCH_GPIO_Port, SEG_SWITCH_4_Pin}
};

/* Seven-segment patterns: bit 0 = A, bit 6 = G. */
static const uint8_t digit_patterns[10] =
{
    0x3FU, /* 0 */
    0x06U, /* 1 */
    0x5BU, /* 2 */
    0x4FU, /* 3 */
    0x66U, /* 4 */
    0x6DU, /* 5 */
    0x7DU, /* 6 */
    0x07U, /* 7 */
    0x7FU, /* 8 */
    0x6FU  /* 9 */
};

static uint8_t current_digit = 0U;

/* ---------- GPIO helper functions ---------- */

static GPIO_PinState logical_to_pin_state(
    uint8_t on,
    uint8_t active_low)
{
    if (active_low != 0U)
    {
        return (on != 0U) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }

    return (on != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}

static void write_pin(
    const GpioPin *gpio,
    uint8_t on,
    uint8_t active_low)
{
    HAL_GPIO_WritePin(
        gpio->port,
        gpio->pin,
        logical_to_pin_state(on, active_low)
    );
}

/* ---------- Initialization ---------- */

void led_display_init(void)
{
    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        led_output_buffer[i] = 0U;
        write_pin(&led_pins[i], 0U, LED_ACTIVE_LOW);
    }

    for (uint32_t i = 0U; i < SEVEN_SEG_DIGIT_COUNT; i++)
    {
        seven_seg_buffer[i] = 0U;
        write_pin(&digit_pins[i], 0U, DIGIT_ACTIVE_LOW);
    }

    /* Clear segments A-G. */
    for (uint32_t i = 0U; i < 7U; i++)
    {
        write_pin(&segment_pins[i], 0U, SEG_ACTIVE_LOW);
    }

    current_digit = 0U;
}

/* ---------- Traffic LED buffer API ---------- */

void led_display_set_led(LedID id, uint8_t on)
{
    if ((uint32_t)id >= LED_COUNT)
        return;

    led_output_buffer[id] = (on != 0U) ? 1U : 0U;
}

void led_display_toggle_led(LedID id)
{
    if ((uint32_t)id >= LED_COUNT)
        return;

    led_output_buffer[id] = (led_output_buffer[id] == 0U) ? 1U : 0U;
}

void led_display_clear_all_leds(void)
{
    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        led_output_buffer[i] = 0U;
    }
}

/* ---------- Seven-segment buffer API ---------- */

void led_display_set_digit(uint8_t position, uint8_t value)
{
    if (position >= SEVEN_SEG_DIGIT_COUNT)
        return;

    if (value > 9U)
        value = 0U;

    seven_seg_buffer[position] = value;
}

void led_display_set_2digit_value(uint8_t left_position, uint8_t value)
{
    if ((left_position + 1U) >= SEVEN_SEG_DIGIT_COUNT)
        return;

    if (value > 99U)
        value = 99U;

    led_display_set_digit(left_position, value / 10U);
    led_display_set_digit(left_position + 1U, value % 10U);
}

/* ---------- Physical traffic LED update ---------- */

static void update_traffic_leds(void)
{
    for (uint32_t i = 0U; i < LED_COUNT; i++)
    {
        write_pin(
            &led_pins[i],
            led_output_buffer[i],
            LED_ACTIVE_LOW
        );
    }
}

/* ---------- Seven-segment multiplexing ---------- */

static void disable_all_digits(void)
{
    for (uint32_t i = 0U; i < SEVEN_SEG_DIGIT_COUNT; i++)
    {
        write_pin(&digit_pins[i], 0U, DIGIT_ACTIVE_LOW);
    }
}

static void update_segments(uint8_t value)
{
    uint8_t pattern = digit_patterns[value];

    for (uint32_t i = 0U; i < 7U; i++)
    {
        uint8_t segment_on = (pattern >> i) & 0x01U;

        write_pin(
            &segment_pins[i],
            segment_on,
            SEG_ACTIVE_LOW
        );
    }
}

static void scan_one_digit(void)
{
    /* Disable digits before changing the segment pattern. */
    disable_all_digits();

    uint8_t value = seven_seg_buffer[current_digit];

    if (value > 9U)
    {
        value = 0U;
    }

    update_segments(value);

    /* Enable only the current digit. */
    write_pin(
        &digit_pins[current_digit],
        1U,
        DIGIT_ACTIVE_LOW
    );

    current_digit++;

    if (current_digit >= SEVEN_SEG_DIGIT_COUNT)
    {
        current_digit = 0U;
    }
}

/* ---------- Main display task ---------- */

void led_display_task(void)
{
    update_traffic_leds();
    scan_one_digit();
}