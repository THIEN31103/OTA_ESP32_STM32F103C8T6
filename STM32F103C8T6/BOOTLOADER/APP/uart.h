#ifndef __UART_H__
#define __UART_H__

#include "stdint.h"
#include "ring_buffer.h"
#include "stm32f1xx_hal.h"

void receiveRX(uint8_t dataRx);
void UART_Init(void);
uint16_t UART_Available(void);
uint8_t UART_Read(void);
	
#endif
