#include "app_uart.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

#define BUF_SIZE 1024

static const char *TAG = "UART";
static QueueHandle_t uart0_queue;
uint8_t __UART_NUM;
uart_rcv_data_callback_t uart_rcv_data_callback = NULL;


static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(BUF_SIZE);
    while(1) {
        //Waiting for UART event.
        if (xQueueReceive(uart0_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, BUF_SIZE);
            switch (event.type) {
            case UART_DATA:
                uint32_t len = uart_read_bytes(__UART_NUM, dtmp, event.size, portMAX_DELAY);
                uart_rcv_data_callback(dtmp, len);
                break;

            case UART_FIFO_OVF:
                ESP_LOGE(TAG, "HW FIFO OVERFLOW");
                uart_flush_input(__UART_NUM);
                xQueueReset(uart0_queue);
                break;

            case UART_BUFFER_FULL:
                ESP_LOGE(TAG, "RING BUFFER FULL");
                uart_flush_input(__UART_NUM);
                xQueueReset(uart0_queue);
                break;

            case UART_BREAK:
                ESP_LOGE(TAG, "UART RX BREAK");
                break;

            case UART_PARITY_ERR:
                ESP_LOGE(TAG, "UART PARITY ERROR");
                break;

            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "UART FRAME ERROR");
                break;
            default:
                ESP_LOGI(TAG, "UART EVENT TYPE: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void app_uart_start(uint8_t UART_NUM,int tx_io_num, int rx_io_num, uint32_t baud_rate)
{
    __UART_NUM = UART_NUM;

    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(UART_NUM, &uart_config);

    uart_set_pin(UART_NUM, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    xTaskCreate(uart_event_task, "uart_event_task", 3072, NULL, 12, NULL);
}

void uart_rcv_set_callback(void *cb)
{
    if(cb)
    {
        uart_rcv_data_callback = cb;
    }
}

void uart_send_data(uint8_t *data, uint8_t len)
{
    uart_write_bytes(__UART_NUM, data, len);
}
