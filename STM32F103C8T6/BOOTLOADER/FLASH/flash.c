#include "flash.h"

void flash_unlock(void)
{
	HAL_FLASH_Unlock();
}

void flash_lock(void)
{
	HAL_FLASH_Lock();
}

void flash_erease(uint32_t addr)
{
	uint32_t PageError;
	FLASH_EraseInitTypeDef ereaseInit;
	ereaseInit.PageAddress = addr;
	ereaseInit.NbPages = 1;
	ereaseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	HAL_FLASHEx_Erase(&ereaseInit, &PageError);
}

void flash_erase_N_pages(uint32_t start_addr, uint32_t num_pages)
{
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t PageError;
    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = start_addr;   
    eraseInit.NbPages     = num_pages;  

    if (HAL_FLASHEx_Erase(&eraseInit, &PageError) != HAL_OK)
    {
			return;
    }
}

/* Write 2 bytes */
void flash_write_arr(uint32_t addr, uint8_t *data, uint16_t len)
{
    uint16_t value;

    for (uint16_t i = 0; i < len; i += 2)
    {
        if (i + 1 < len)
        {
            value = data[i] | ((uint16_t)data[i + 1] << 8);
        }
        else
        {
            value = data[i] | (0xFF << 8);
        }

        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr + i, value);
    }
}


void flash_read_arr(uint32_t addr, uint8_t *data, uint16_t len)
{
	for(uint16_t i = 0; i < len; i +=2)
	{
//		volatile uint32_t *p = (volatile uint32_t*) (addr + i);
//		uint16_t data_tmp = *p;
		uint16_t data_tmp = *(volatile uint32_t*)(addr +i);
		
//		data[i] = data_tmp;
//		data[i + 1] = data_tmp >> 8;
		*(data + i) = data_tmp;
		*(data + i + 1) = data_tmp >> 8;
	}
}
