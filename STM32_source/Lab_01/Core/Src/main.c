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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
GPIO_TypeDef *SEG7_PORT_1 = GPIOA;
GPIO_TypeDef *SEG7_PORT_2 = GPIOB;

uint16_t SEG7_1[] = {
    0x0100, // PA8
    0x0200, // PA9
    0x0400, // PA10
    0x0800, // PA11
    0x1000, // PA12
    0x2000, // PA13
    0x4000  // PA14
};

uint16_t SEG7_2[] = {
    0x0001, // PB0
    0x0002, // PB1
    0x0004, // PB2
    0x0008, // PB3
    0x0010, // PB4
    0x0020, // PB5
    0x0040  // PB6
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void SEG7_Display(GPIO_TypeDef *GPIOx, uint16_t *SEG7, uint8_t number);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SEG7_Display(GPIO_TypeDef *GPIOx, uint16_t *SEG7, uint8_t number)
{
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int counter_traffic_1 = 0, counter_traffic_2 = 0;
  while (1)
  {
      /* =====================================================
         Traffic Light 2 = GREEN
         Traffic Light 1 = RED
         ===================================================== */

      // Light 1 = RED
      HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(YELLOW_1_GPIO_Port, YELLOW_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_RESET);

      // Light 2 = GREEN
      HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YELLOW_2_GPIO_Port, YELLOW_2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_SET);

      // Countdown
      counter_traffic_1 = 5, counter_traffic_2 = 3;
      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);


      /* =====================================================
         Traffic Light 2 = YELLOW
         Traffic Light 1 = RED
         ===================================================== */

      HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YELLOW_2_GPIO_Port, YELLOW_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_RESET);

      counter_traffic_2 = 2;
      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);


      /* =====================================================
         Traffic Light 1 = GREEN
         Traffic Light 2 = RED
         ===================================================== */

      // Light 2 = RED
      HAL_GPIO_WritePin(RED_2_GPIO_Port, RED_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(YELLOW_2_GPIO_Port, YELLOW_2_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GREEN_2_GPIO_Port, GREEN_2_Pin, GPIO_PIN_RESET);

      // Light 1 = GREEN
      HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YELLOW_1_GPIO_Port, YELLOW_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_SET);

      // Countdown
      counter_traffic_1 = 3, counter_traffic_2 = 5;
      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);


      /* =====================================================
         Traffic Light 1 = YELLOW
         Traffic Light 2 = RED
         Countdown: 2 -> 1
         ===================================================== */

      HAL_GPIO_WritePin(RED_1_GPIO_Port, RED_1_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(YELLOW_1_GPIO_Port, YELLOW_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GREEN_1_GPIO_Port, GREEN_1_Pin, GPIO_PIN_RESET);

      // Countdown
      counter_traffic_1 = 2;
      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);

      SEG7_Display(GPIOA, SEG7_1, --counter_traffic_1);
      SEG7_Display(GPIOB, SEG7_2, --counter_traffic_2);
      HAL_Delay(1000);
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
  HAL_GPIO_WritePin(GPIOA, RED_1_Pin|YELLOW_1_Pin|GREEN_1_Pin|RED_2_Pin
                          |YELLOW_2_Pin|GREEN_2_Pin|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : RED_1_Pin YELLOW_1_Pin GREEN_1_Pin RED_2_Pin
                           YELLOW_2_Pin GREEN_2_Pin PA8 PA9
                           PA10 PA11 PA12 PA13
                           PA14 */
  GPIO_InitStruct.Pin = RED_1_Pin|YELLOW_1_Pin|GREEN_1_Pin|RED_2_Pin
                          |YELLOW_2_Pin|GREEN_2_Pin|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3
                           PB4 PB5 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
