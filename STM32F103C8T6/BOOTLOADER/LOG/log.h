#ifndef __LOG_H__
#define __LOG_H__

#include "stm32f1xx_hal.h"
#include "stdio.h"
#include "stdarg.h"
#include "stdint.h"

void mPrintf(const char *format, ...);
void LOG_Init(UART_HandleTypeDef *huart);

#endif
