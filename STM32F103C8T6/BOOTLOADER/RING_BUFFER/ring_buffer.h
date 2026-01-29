#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__
#include <stdint.h>

typedef struct 
{
	uint8_t *buffer;
	int head;
	int tail;
	uint16_t maxlen;
}RingBuffer_t;

void RingBuffer_Init(RingBuffer_t *ring_buff, uint8_t *buff, uint16_t len);
int8_t RingBuffer_Push(RingBuffer_t *ring_buff, uint8_t data);
int8_t RingBuffer_Pop(RingBuffer_t *ring_buff, uint8_t *data);
uint16_t RingBuffer_Availabel(RingBuffer_t *ring_buff);
#endif
