/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUM_OF_LED 1
#define NUM_OF_SEG 4
#define TIMER_CYCLE 10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
static const uint8_t digit[10] =
{
	0x3F, // 0 = abcdef
	0x06, // 1 = bc
	0x5B, // 2 = abdeg
	0x4F, // 3 = abcdg
	0x66, // 4 = bcfg
	0x6D, // 5 = acdfg
	0x7D, // 6 = acdefg
	0x07, // 7 = abc
	0x7F, // 8 = abcdefg
	0x6F  // 9 = abcdfg
};

static const uint8_t DOT_START = 100;
static const uint8_t LED_START = 50;
//static const uint8_t SEG_START = 50;

uint16_t SEG7[] = {
    0x0001, // PB0
    0x0002, // PB1
    0x0004, // PB2
    0x0008, // PB3
    0x0010, // PB4
    0x0020, // PB5
    0x0040  // PB6
};

uint16_t EN_PIN_PA[] = {
    0x0040, // PA6 (GPIO_PIN_6)
    0x0080, // PA7 (GPIO_PIN_7)
	0x0100, // PA8 (GPIO_PIN_8)
	0x0200  // PA9 (GPIO_PIN_9)
};

int led_buffer[] = {1, 2, 3, 0};

uint16_t DOT_PIN_PA = 0x0010;  // PA4 (GPIO_PIN_4)
uint16_t LED_PIN_PA = 0x0020;  // PA5 (GPIO_PIN_5)

uint8_t DOT_COUNTDOWN = 0;
uint8_t DOT_FLAG = 0;

uint8_t LED_COUNTDOWN = 0;
uint8_t LED_FLAG = 0;

uint8_t SEG_COUNTDOWN = 0;
uint8_t SEG_FLAG = 0;

uint8_t EN_PIN_COUNT = 0;

uint8_t timer_counter[2] = {0};
uint8_t timer_flag[2] = {0};

uint8_t hour = 15, minute = 8, second = 50;

GPIO_TypeDef* COL_PORTS[8] =
{
    GPIOA, GPIOA, GPIOA, GPIOA,
    GPIOA, GPIOA, GPIOA, GPIOA
};

uint16_t COL_PINS[8] =
{
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15
};

uint16_t ROW_PINS[8] =
{
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15
};

const int MAX_LED_MATRIX = 8;
int index_led_matrix = 0;
int offset_led_matrix = 0;
uint8_t matrix_buffer[8] =
{
    0b00000000,
    0b00000000,
    0b00111100,
    0b01000010,
    0b01111110,
    0b01000010,
    0b01000010,
	0b00000000,
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void SEG7_Display(GPIO_TypeDef *GPIOx, uint16_t *SEG7, uint8_t number);

void update7SEG(int index);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SEG7_Display(GPIO_TypeDef *GPIOx, uint16_t *SEG7, uint8_t number)
// legacy code, this was for 14 pins just for 2 leds
{
    if (number > 9)
        return;

    for (int i = 0; i < 7; i++)
    {
        if (digit[number] & (1 << i))
            HAL_GPIO_WritePin(GPIOx, SEG7[i], GPIO_PIN_RESET); // ON
        else
            HAL_GPIO_WritePin(GPIOx, SEG7[i], GPIO_PIN_SET);   // OFF
    }
}

void update7SEG(int index)
{
	for (uint8_t i = 0; i < NUM_OF_SEG; ++i)
		HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[i], GPIO_PIN_SET);

	SEG7_Display(GPIOB, SEG7, led_buffer[index]);
	HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[index], GPIO_PIN_RESET);
}

void updateClockBuffer()
{
	led_buffer[0] = hour/10;
	led_buffer[1] = hour%10;
	led_buffer[2] = minute/10;
	led_buffer[3] = minute%10;
}

void setTimer(int duration, int index)
{
	timer_counter[index] = duration/TIMER_CYCLE;
	timer_flag[index] = 0;
}

void timer_run(int index)
{
	if (timer_counter[index] > 0)
	{
		--timer_counter[index];
		if (timer_counter[index] == 0) timer_flag[index] = 1;
	}
}


void updateLEDMatrix(uint8_t index)
{
    if (index >= MAX_LED_MATRIX)
        return;

    for (uint8_t col = 0; col < 8; col++)
    {
        HAL_GPIO_WritePin(GPIOA, COL_PINS[col], GPIO_PIN_SET);
    }

    for (uint8_t row = 0; row < 8; row++)
    {
        uint8_t led_on = (matrix_buffer[index] >> ((row + offset_led_matrix) % 8)) & 0x01;

        if (led_on)
        {
            HAL_GPIO_WritePin(GPIOB, ROW_PINS[row], GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOB, ROW_PINS[row], GPIO_PIN_SET);
        }
    }

    HAL_GPIO_WritePin(GPIOA, COL_PINS[index], GPIO_PIN_RESET);
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
  MX_TIM2_Init();
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);

  // init the seg7
  //  for (uint8_t i = 0; i < NUM_OF_LED; ++i)
  //	  HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[i], GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  EN_PIN_COUNT = NUM_OF_SEG;
  updateClockBuffer();
  int index_led_matrix = 0;
  setTimer(10, 0);
  setTimer(10, 1);
  while (1)
  {
	  if (LED_FLAG == 1)
	  {
		  HAL_GPIO_TogglePin(GPIOA, LED_PIN_PA);
		  LED_FLAG = 0;
		  LED_COUNTDOWN = LED_START;
	  };

	  if (DOT_FLAG == 1)
	  {
		  HAL_GPIO_TogglePin(GPIOA, DOT_PIN_PA);
		  DOT_FLAG = 0;
		  DOT_COUNTDOWN = DOT_START;
	  };

	  if (timer_flag[0] == 1)
	  {
		  ++second;
		  if (second >= 60)
		  {
			  second = 0;
			  ++minute;
		  };

		  if (minute >= 60)
		  {
			  minute = 0;
			  ++hour;
		  };

		  if (hour >= 24)
		  {
			  hour = 0;
		  };

		  updateClockBuffer();
		  ++EN_PIN_COUNT;
		  if (EN_PIN_COUNT >= NUM_OF_SEG)
			  EN_PIN_COUNT = 0;

		  update7SEG(EN_PIN_COUNT);

		  setTimer(250, 0);
	  };

	  if (timer_flag[1] == 1)
	  {
		  updateLEDMatrix(index_led_matrix);

		  index_led_matrix++;

		  if (index_led_matrix >= MAX_LED_MATRIX)
		  {
			  index_led_matrix = 0;

			  ++offset_led_matrix;

			  if (offset_led_matrix >= MAX_LED_MATRIX)
				  offset_led_matrix = 0;
		  }

		  setTimer(10, 1);
	  };
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5
                           PA6 PA7 PA8 PA9
                           PA10 PA11 PA12 PA13
                           PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB6 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
	timer_run(0);
	timer_run(1);

	if (LED_COUNTDOWN > 0)
		--LED_COUNTDOWN;
	if (LED_COUNTDOWN <= 0)
		LED_FLAG = 1;

	if (SEG_COUNTDOWN > 0)
		--SEG_COUNTDOWN;
	if (SEG_COUNTDOWN <= 0)
		SEG_FLAG = 1;

	if (DOT_COUNTDOWN > 0)
		--DOT_COUNTDOWN;
	if (DOT_COUNTDOWN <= 0)
		DOT_FLAG = 1;
}
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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
