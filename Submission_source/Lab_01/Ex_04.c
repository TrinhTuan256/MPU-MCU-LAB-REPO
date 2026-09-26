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
void SystemClock_Config(void); // system code not me
static void MX_GPIO_Init(void); // also system code not me
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
  int counter = 0;
  while (1)
  {
	  if (counter >= 10)
		  counter = 0;
	  SEG7_Display(SEG7_PORT_1, SEG7_1, counter++);
	  HAL_Delay(1000);
  };

  /* USER CODE END 3 */
}