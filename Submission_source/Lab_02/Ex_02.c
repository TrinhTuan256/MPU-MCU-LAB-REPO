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
  
  	  if (SEG_FLAG == 1)
  	  {
  		  HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[EN_PIN_COUNT], GPIO_PIN_SET);
  		  ++EN_PIN_COUNT;
  		  if (EN_PIN_COUNT >= NUM_OF_SEG)
  			  EN_PIN_COUNT = 0;
  		  HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[EN_PIN_COUNT], GPIO_PIN_RESET);
  
  		  SEG7_Display(GPIOB, SEG7, SEG_DISPLAY[EN_PIN_COUNT]);
  		  SEG_FLAG = 0;
  		  SEG_COUNTDOWN = SEG_START;
  	  };
      /* USER CODE END WHILE */
  
      /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
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
