#ifndef __PROCESS_FILE_HEX_H__
#define __PROCESS_FILE_HEX_H__

#include"stdint.h"
#include"stdio.h"

typedef enum
{
    CRC_OK,
    CRC_ERROR
} CRC_Status_t;

void convert_string_to_array_hex(char *input, uint8_t *output);
uint8_t check_sum(uint8_t *buff, uint8_t len);
void swap_4_byte(uint8_t *data, uint8_t len);
void print_data(uint8_t *data, uint8_t len);

#endif