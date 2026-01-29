#include "stm32f1xx_hal.h"
#include "min.h"

extern UART_HandleTypeDef huart1;

void min_tx_start(uint8_t port)
{
	
}

void min_tx_finished(uint8_t port)
{
	
}

void min_tx_byte(uint8_t port, uint8_t byte)
{
	HAL_UART_Transmit(&huart1, &byte, 1, 100);
}

uint16_t min_tx_space(uint8_t port)
{
	return 512;
}

uint32_t min_time_ms(void)
{
	return HAL_GetTick();
}
