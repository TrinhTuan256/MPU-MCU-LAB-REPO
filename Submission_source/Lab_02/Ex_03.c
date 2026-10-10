/* USER CODE BEGIN PD */
#define NUM_OF_LED 1
#define NUM_OF_SEG 4
/* USER CODE END PD */

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
static const uint8_t SEG_START = 50;

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

uint16_t SEG_DISPLAY[] = {1, 2, 3, 0};

uint16_t DOT_PIN_PA = 0x0010;  // PA4 (GPIO_PIN_4)
uint16_t LED_PIN_PA = 0x0020;  // PA5 (GPIO_PIN_5)

uint8_t DOT_COUNTDOWN = 0;
uint8_t DOT_FLAG = 0;

uint8_t LED_COUNTDOWN = 0;
uint8_t LED_FLAG = 0;

uint8_t SEG_COUNTDOWN = 0;
uint8_t SEG_FLAG = 0;

uint8_t EN_PIN_COUNT = 0;
/* USER CODE END PV */

void update7SEG(int index)
{
	switch (index)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[index], GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[index], GPIO_PIN_RESET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[index], GPIO_PIN_RESET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOA, EN_PIN_PA[index], GPIO_PIN_RESET);
			break;
		default :
			break;
	}
}

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
		  {
			  update7SEG(EN_PIN_COUNT);
			  EN_PIN_COUNT = 0;
		  };

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
