#ifndef __FLASH_H__
#define __FLASH_H__
#include "stm32f1xx_hal.h"

void flash_erase_N_pages(uint32_t start_addr, uint32_t num_pages);
void flash_write_arr(uint32_t addr, uint8_t *data, uint16_t len);
void flash_read_arr(uint32_t addr, uint8_t *data, uint16_t len);
void flash_unlock(void);
void flash_lock(void);
#endif
