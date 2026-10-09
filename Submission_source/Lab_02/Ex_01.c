uint16_t EN_PIN_PA[] = {
    0x0040, // PA6 (GPIO_PIN_6)
    0x0080  // PA7 (GPIO_PIN_7)
};

uint16_t LED_PIN_PA = 0x0020;  // PA5 (GPIO_PIN_5)

uint8_t LED_COUNTDOWN = 0;
uint8_t LED_FLAG = 0;

uint8_t SEG_COUNTDOWN = 0;
uint8_t SEG_FLAG = 0;

uint8_t EN_PIN_COUNT = 0;

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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  EN_PIN_COUNT = 1;
  while (1)
  {
	  if (LED_FLAG == 1)
	  {
		  HAL_GPIO_TogglePin(GPIOA, LED_PIN_PA);
		  LED_FLAG = 0;
		  LED_COUNTDOWN = LED_START;
	  };

	  if (SEG_FLAG == 1)
	  {
		  HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[EN_PIN_COUNT], GPIO_PIN_SET);
		  ++EN_PIN_COUNT;
		  if (EN_PIN_COUNT >= 2)
			  EN_PIN_COUNT = 0;
		  HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[EN_PIN_COUNT], GPIO_PIN_RESET);

		  SEG7_Display(GPIOB, SEG7, EN_PIN_COUNT + 1);
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
}
