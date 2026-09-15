/* USER CODE BEGIN PV */
uint16_t LED[] = {
    0x0010,  // PA4
    0x0020,  // PA5
    0x0040,  // PA6
    0x0080,  // PA7
    0x0100,  // PA8
    0x0200,  // PA9
    0x0400,  // PA10
    0x0800,  // PA11
    0x1000,  // PA12
    0x2000,  // PA13
    0x4000,  // PA14
    0x8000   // PA15
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  int counter = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  HAL_GPIO_WritePin(GPIOA, LED[counter], GPIO_PIN_RESET);
	  counter++;
	  if (counter == 12)
		  counter = 0;

	  HAL_GPIO_WritePin(GPIOA, LED[counter], GPIO_PIN_SET);
	  HAL_Delay(100);

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
