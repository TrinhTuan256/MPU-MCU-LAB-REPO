/* USER CODE BEGIN 0 */
void clearAllClock(void)
{
    for (int i = 0; i < 12; i++)
    {
        HAL_GPIO_WritePin(GPIOA, LED[i], GPIO_PIN_RESET);
    }
}

void setNumberOnClock(int num)
{
    if (num >= 0 && num < 12)
    {
        HAL_GPIO_WritePin(GPIOA, LED[num], GPIO_PIN_SET);
    }
}

void clearNumberOnClock(int num)
{
    if (num >= 0 && num < 12)
    {
        HAL_GPIO_WritePin(GPIOA, LED[num], GPIO_PIN_RESET);
    }
}

void updateClock(uint8_t hour, uint8_t minute, uint8_t second)
{
    // Turn everything off first
    clearAllClock();

    // Convert time to 12 LED positions
    int hourPosition = hour;
    int minutePosition = minute / 5;
    int secondPosition = second / 5;

    // Turn on the corresponding LEDs
    setNumberOnClock(hourPosition);
    setNumberOnClock(minutePosition);
    setNumberOnClock(secondPosition);
}
/* USER CODE END 0 */

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
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      updateClock(hour, minute, second);

      HAL_Delay(100);

      second++;

      if (second >= 60)
      {
          second = 0;
          minute++;

          if (minute >= 60)
          {
              minute = 0;
              hour++;

              if (hour >= 12)
              {
                  hour = 0;
              }
          }
      }
  }
  /* USER CODE END 3 */
}
