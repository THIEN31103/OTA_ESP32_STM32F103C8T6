#include "ring_buffer.h"

int8_t RingBuffer_Push(RingBuffer_t *ring_buff, uint8_t data)
{
	uint16_t  next;
	
	next = ring_buff->head + 1; /* Next is where head will point to affter this write*/
	if(next >= ring_buff->maxlen)
	{
		next = 0;
	}
	
	if(next == ring_buff->tail)
	{
		return -1;
	}
	
	ring_buff->buffer[ring_buff->head] = data; /* Load data and then move*/
	ring_buff->head = next;	/* Head to next offset*/
	return 0;
}

int8_t RingBuffer_Pop(RingBuffer_t *ring_buff, uint8_t *data)
{
	uint16_t next;
	
	if(ring_buff->head == ring_buff->tail) /* If the head == tail, we don't have any data*/
	{
		return -1;
	}
	
	next = ring_buff->tail + 1; /* Next is where tail will point to after this read*/
	if(next >= ring_buff->maxlen)
	{
		next = 0;
	}
	
	*data = ring_buff->buffer[ring_buff->tail]; /* Read data and then move*/
	ring_buff->tail = next;											/* Tail to next offset */
	return 0;
}

uint16_t RingBuffer_Availabel(RingBuffer_t *ring_buff)
{
	if(ring_buff->head < ring_buff->tail)
	{
		return ring_buff->maxlen - (ring_buff->tail - ring_buff->head);
	}
	return (ring_buff->tail - ring_buff->head);
}

void RingBuffer_Init(RingBuffer_t *ring_buff, uint8_t *buff, uint16_t len)
{
	ring_buff->buffer = buff;
	ring_buff->head = 0;
	ring_buff->tail = 0;
	ring_buff->maxlen = len;
}
