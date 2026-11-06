/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "timer.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "input_reading.h"
#include "input_processing.h"
#include "led_display.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    MODE_NORMAL = 1,
    MODE_MODIFY_RED = 2,
    MODE_MODIFY_AMBER = 3,
    MODE_MODIFY_GREEN = 4
} AppMode;

typedef enum
{
    PHASE_EW_RED_NS_GREEN = 0,
    PHASE_EW_RED_NS_AMBER,
    PHASE_EW_GREEN_NS_RED,
    PHASE_EW_AMBER_NS_RED
} TrafficPhase;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
static uint8_t current_mode = MODE_NORMAL;

/* Saved durations in seconds (default: Red=5s, Amber=2s, Green=3s) */
static uint8_t red_duration = 5U;
static uint8_t amber_duration = 2U;
static uint8_t green_duration = 3U;

/* Normal mode state */
static TrafficPhase current_phase = PHASE_EW_RED_NS_GREEN;
static uint8_t countdown_ew = 5U;
static uint8_t countdown_ns = 3U;

/* Modify mode state */
static uint8_t temp_duration = 5U;
static uint8_t blink_state = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void normal_mode_init(void);
void normal_mode(void);
void modify_mode(uint8_t mode);
void switch_mode(uint8_t new_mode);
static void update_traffic_leds_for_phase(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void update_traffic_leds_for_phase(void)
{
    led_display_clear_all_leds();
    switch (current_phase)
    {
        case PHASE_EW_RED_NS_GREEN:
            led_display_set_led(LED_EW_RED, 1U);
            led_display_set_led(LED_NS_GREEN, 1U);
            break;

        case PHASE_EW_RED_NS_AMBER:
            led_display_set_led(LED_EW_RED, 1U);
            led_display_set_led(LED_NS_AMBER, 1U);
            break;

        case PHASE_EW_GREEN_NS_RED:
            led_display_set_led(LED_EW_GREEN, 1U);
            led_display_set_led(LED_NS_RED, 1U);
            break;

        case PHASE_EW_AMBER_NS_RED:
            led_display_set_led(LED_EW_AMBER, 1U);
            led_display_set_led(LED_NS_RED, 1U);
            break;
    }
}

void normal_mode_init(void)
{
    current_phase = PHASE_EW_RED_NS_GREEN;
    countdown_ew = red_duration;
    countdown_ns = green_duration;
    update_traffic_leds_for_phase();
    led_display_set_2digit_value(0U, countdown_ew);
    led_display_set_2digit_value(2U, countdown_ns);
    timer_set(TIMER_COUNTDOWN, 1000U);
}

void switch_mode(uint8_t new_mode)
{
    current_mode = new_mode;
    led_display_clear_all_leds();

    if (new_mode == MODE_NORMAL)
    {
        normal_mode_init();
    }
    else
    {
        /* Load current saved duration as initial modify value */
        if (new_mode == MODE_MODIFY_RED)
        {
            temp_duration = red_duration;
        }
        else if (new_mode == MODE_MODIFY_AMBER)
        {
            temp_duration = amber_duration;
        }
        else if (new_mode == MODE_MODIFY_GREEN)
        {
            temp_duration = green_duration;
        }

        blink_state = 1U;
        timer_set(TIMER_BLINK, 250U); /* 2 Hz blinking: 250 ms toggle */
        led_display_set_2digit_value(0U, new_mode);
        led_display_set_2digit_value(2U, temp_duration);
    }
}

/* Normal Mode: Traffic light sequencing and countdown display */
void normal_mode(void)
{
    if (timer_read_and_clear(TIMER_COUNTDOWN))
    {
        timer_set(TIMER_COUNTDOWN, 1000U);

        if (countdown_ew > 0U) countdown_ew--;
        if (countdown_ns > 0U) countdown_ns--;

        switch (current_phase)
        {
            case PHASE_EW_RED_NS_GREEN:
                if (countdown_ns == 0U)
                {
                    current_phase = PHASE_EW_RED_NS_AMBER;
                    countdown_ns = amber_duration;
                    update_traffic_leds_for_phase();
                }
                break;

            case PHASE_EW_RED_NS_AMBER:
                if ((countdown_ns == 0U) || (countdown_ew == 0U))
                {
                    current_phase = PHASE_EW_GREEN_NS_RED;
                    countdown_ew = green_duration;
                    countdown_ns = red_duration;
                    update_traffic_leds_for_phase();
                }
                break;

            case PHASE_EW_GREEN_NS_RED:
                if (countdown_ew == 0U)
                {
                    current_phase = PHASE_EW_AMBER_NS_RED;
                    countdown_ew = amber_duration;
                    update_traffic_leds_for_phase();
                }
                break;

            case PHASE_EW_AMBER_NS_RED:
                if ((countdown_ew == 0U) || (countdown_ns == 0U))
                {
                    current_phase = PHASE_EW_RED_NS_GREEN;
                    countdown_ew = red_duration;
                    countdown_ns = green_duration;
                    update_traffic_leds_for_phase();
                }
                break;
        }

        led_display_set_2digit_value(0U, countdown_ew);
        led_display_set_2digit_value(2U, countdown_ns);
    }
}

/* Modify Mode: Single function handling Modes 2, 3, and 4 via parameter */
void modify_mode(uint8_t mode)
{
    LedID led_ew, led_ns;
    uint8_t *target_duration;

    switch (mode)
    {
        case MODE_MODIFY_RED:
            led_ew = LED_EW_RED;
            led_ns = LED_NS_RED;
            target_duration = &red_duration;
            break;

        case MODE_MODIFY_AMBER:
            led_ew = LED_EW_AMBER;
            led_ns = LED_NS_AMBER;
            target_duration = &amber_duration;
            break;

        case MODE_MODIFY_GREEN:
            led_ew = LED_EW_GREEN;
            led_ns = LED_NS_GREEN;
            target_duration = &green_duration;
            break;

        default:
            return;
    }

    /* 2 Hz LED blinking (250 ms toggle interval) */
    if (timer_read_and_clear(TIMER_BLINK))
    {
        timer_set(TIMER_BLINK, 250U);
        blink_state = (blink_state == 0U) ? 1U : 0U;
        led_display_clear_all_leds();
        if (blink_state != 0U)
        {
            led_display_set_led(led_ew, 1U);
            led_display_set_led(led_ns, 1U);
        }
    }

    /* Button 2: Increase duration (1 - 99, with auto-increment on hold) */
    if (input_get_press_event(BUTTON_2_INDEX))
    {
        temp_duration++;
        if (temp_duration > 99U)
        {
            temp_duration = 1U;
        }
        led_display_set_2digit_value(2U, temp_duration);
    }

    /* Button 3: Set/confirm the chosen value */
    if (input_get_press_event(BUTTON_3_INDEX))
    {
        *target_duration = temp_duration;
    }

    /* Display Mode on digits 0 & 1, Value on digits 2 & 3 */
    led_display_set_2digit_value(0U, mode);
    led_display_set_2digit_value(2U, temp_duration);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  timer_set(TIMER_BUTTON, TIMER_TICK_MS);
  led_display_init();
  timer_set(TIMER_DISPLAY, 3U);
  switch_mode(MODE_NORMAL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Periodic button scanning task (every 10 ms) */
    if (timer_read_and_clear(TIMER_BUTTON))
    {
      timer_set(TIMER_BUTTON, TIMER_TICK_MS);
      button_reading();
      input_processing();
    }

    /* Periodic 7-segment multiplexing & LED refresh task (every 3 ms) */
    if (timer_read_and_clear(TIMER_DISPLAY))
    {
      timer_set(TIMER_DISPLAY, 3U);
      led_display_task();
    }

    /* Button 1: Mode cycling (1 -> 2 -> 3 -> 4 -> 1) */
    if (input_get_press_event(BUTTON_1_INDEX))
    {
      current_mode++;
      if (current_mode > MODE_MODIFY_GREEN)
      {
        current_mode = MODE_NORMAL;
      }
      switch_mode(current_mode);
    }

    /* Run current active mode handler */
    if (current_mode == MODE_NORMAL)
    {
      normal_mode();
    }
    else
    {
      modify_mode(current_mode);
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB3
                           PB4 PB5 PB6 PB7
                           PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA11 PA12 PA13 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2)
  {
    timer_tick();
  };
};
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
