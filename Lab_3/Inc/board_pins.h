#ifndef BOARD_PINS_H
#define BOARD_PINS_H

#include "main.h"

/* ---------- GPIO port definitions ---------- */

#define BUTTON_GPIO_Port       GPIOA
#define LED_EW_GPIO_Port       GPIOA

#define SEG_GPIO_Port          GPIOB
#define SEG_SWITCH_GPIO_Port   GPIOB
#define LED_NS_GPIO_Port       GPIOB

/* ---------- Buttons: GPIOA ---------- */

#define BUTTON_1_Pin           GPIO_PIN_1
#define BUTTON_2_Pin           GPIO_PIN_2
#define BUTTON_3_Pin           GPIO_PIN_3

/* ---------- East-West traffic LEDs: GPIOA ---------- */

#define LED_EW_RED_Pin         GPIO_PIN_11
#define LED_EW_AMBER_Pin       GPIO_PIN_12
#define LED_EW_GREEN_Pin       GPIO_PIN_13

/* ---------- Seven-segment A-G: GPIOB ---------- */

#define SEG_A_Pin              GPIO_PIN_0
#define SEG_B_Pin              GPIO_PIN_1
#define SEG_C_Pin              GPIO_PIN_2
#define SEG_D_Pin              GPIO_PIN_3
#define SEG_E_Pin              GPIO_PIN_4
#define SEG_F_Pin              GPIO_PIN_5
#define SEG_G_Pin              GPIO_PIN_6

/* ---------- Four digit-select outputs: GPIOB ---------- */

#define SEG_SWITCH_1_Pin       GPIO_PIN_7
#define SEG_SWITCH_2_Pin       GPIO_PIN_8
#define SEG_SWITCH_3_Pin       GPIO_PIN_9
#define SEG_SWITCH_4_Pin       GPIO_PIN_10

/* ---------- North-South traffic LEDs: GPIOB ---------- */

#define LED_NS_RED_Pin         GPIO_PIN_11
#define LED_NS_AMBER_Pin       GPIO_PIN_12
#define LED_NS_GREEN_Pin       GPIO_PIN_13

#endif