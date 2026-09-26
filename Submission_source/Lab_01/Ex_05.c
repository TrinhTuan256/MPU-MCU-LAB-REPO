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