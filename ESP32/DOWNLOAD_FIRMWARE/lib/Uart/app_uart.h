#ifndef __APP_UART_H__
#define __APP_UART_H__
#include"stdint.h"

typedef void (*uart_rcv_data_callback_t)(uint8_t *data, uint32_t len);
void app_uart_start(uint8_t UART_NUM,int tx_io_num, int rx_io_num, uint32_t baud_rate);
void uart_send_data(uint8_t *data, uint8_t len);
void uart_rcv_set_callback(void *cb);

#endif
