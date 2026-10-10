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
        uint8_t led_on = (matrix_buffer[index] >> ((row) % 8)) & 0x01;

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
