#include "log.h"

UART_HandleTypeDef *huart_log;
char buff[40];

void mPrintf(const char *format, ...)
{
	for(uint8_t i = 0; i < 40; i++)
	{
		buff[i] = 0;
	}
	va_list args;
	va_start(args, format);
	vsnprintf(buff, sizeof(buff), format, args);
	HAL_UART_Transmit(huart_log,(uint8_t*) buff, 40, 100);
}

void LOG_Init(UART_HandleTypeDef *huart)
{
	huart_log = huart;
}
