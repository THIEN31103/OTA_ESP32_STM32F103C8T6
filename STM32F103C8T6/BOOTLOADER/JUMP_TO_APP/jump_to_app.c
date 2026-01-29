#include "jump_to_app.h"

run_app_handler run_app;
void run_app_program(uint32_t addr_app)
{
    /* Turn off Peripheral, Clear Interrupt Flag */
    HAL_RCC_DeInit();
    /* Clear Pending Interrupt Request, turn off System Tick */
    HAL_DeInit(); /* Turn off fault handler */
    SCB->SHCSR = ( SCB_SHCSR_USGFAULTENA_Msk |
                   SCB_SHCSR_BUSFAULTENA_Msk |
                   SCB_SHCSR_MEMFAULTENA_Msk );

    /* Set Main Stack Pointer */
    __set_MSP(*(volatile uint32_t*) addr_app);
    run_app = (run_app_handler)(*(volatile uint32_t*)(addr_app + 4));
    run_app();
}