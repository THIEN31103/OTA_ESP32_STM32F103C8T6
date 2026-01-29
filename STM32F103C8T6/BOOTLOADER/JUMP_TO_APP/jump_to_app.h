#ifndef __JUMP_TO_APP_H__
#define __JUMP_TO_APP_H__
#include "stm32f1xx_hal.h"

typedef void (*run_app_handler)(void);
void run_app_program(uint32_t addr_app);

#endif
