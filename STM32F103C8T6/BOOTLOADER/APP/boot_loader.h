#ifndef __BOOT_LOADER_H__
#define __BOOT_LOADER_H__

#include "stm32f1xx_hal.h"
#include "bootloader_command.h"

typedef enum
{
  OTA_IDLE_STATE,
  OTA_START_STATE,
  OTA_SEND_INFOR_STATE,
  OTA_SEND_DATA_STATE,
  OTA_END_STATE
}OTA_State_Typedef;

typedef enum
{
	NO_UPDATE,
	DONE_UPDATE
}OTA_State_Update_t;

OTA_State_Update_t is_updated(void);

void bootloader_init(void);
void bootloader_request_update(void);
void bootloader_handle(void);

void bootloader_send_data(void *data, uint8_t len);
void ota_send_code(OTA_Code_Name ota_code);
void ota_send_respond(OTA_Respond_Name ack);

#endif

