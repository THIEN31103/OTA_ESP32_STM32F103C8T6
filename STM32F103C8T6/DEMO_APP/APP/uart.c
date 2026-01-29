#include "uart.h"

#define RING_UART_LEN	128
static uint8_t buff_uart[RING_UART_LEN];
RingBuffer_t ring_buff;


void receiveRX(uint8_t dataRx)
{
//	buff_uart[index++] = dataRx;
	RingBuffer_Push(&ring_buff, dataRx);
}

uint16_t UART_Available()
{
	return RingBuffer_Availabel(&ring_buff);
}

uint8_t UART_Read()
{
	uint8_t data;
	RingBuffer_Pop(&ring_buff, &data);
	return data;
}

void UART_Init(void)
{
	RingBuffer_Init(&ring_buff, buff_uart, RING_UART_LEN);
}
