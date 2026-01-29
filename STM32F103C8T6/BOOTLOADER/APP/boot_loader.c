#include "boot_loader.h"
#include "uart.h"
#include "min.h"
#include "bootloader_command.h"
#include "flash.h"
#define MIN_PORT	0
#define MIN_ID		1
#define START_ADDRESS 0x08005000
struct min_context min_ctx;
OTA_State_Typedef ota_state;

static char name[50];
static char version[20];
static uint32_t length = 0;
static OTA_State_Update_t update_state = NO_UPDATE;

#define OTA_BUF_SIZE 256

void bootloader_send_data(void *data, uint8_t len)
{
  min_send_frame(&min_ctx, MIN_ID,(uint8_t*)data, len);
}

void ota_send_code(OTA_Code_Name ota_code)
{
  OTACode cmd;
  cmd.command_id = OTA_CODE;
  cmd.len = 1;
  cmd.ota_code = ota_code;
  bootloader_send_data(&cmd, sizeof(cmd));
}

void ota_send_respond(OTA_Respond_Name ack)
{
  OTARespond respond;
  respond.command_id = OTA_RESPOND;
  respond.len = 1;
  respond.ack = ack;
  bootloader_send_data(&respond, sizeof(respond));
}

void min_application_handler(uint8_t min_id, uint8_t const *min_payload, 
														 uint8_t len_payload, uint8_t port)
{
	switch(ota_state)
  {
    case OTA_IDLE_STATE:
		{	
        OTACode *ota_code = (OTACode*)min_payload;
        if(ota_code->command_id == OTA_CODE && ota_code->ota_code == REQUEST_CODE)
        {
          ota_state = OTA_START_STATE;
          ota_send_respond(ACK);
        }
				else if(ota_code->command_id == OTA_CODE && ota_code->ota_code == NOT_NEW_VERSION )
				{
					ota_state = OTA_IDLE_STATE;
				}
		}
      break;

    case OTA_START_STATE:
		{
			OTACode *ota_code = (OTACode*)min_payload;
			if(ota_code->command_id == OTA_CODE && ota_code->ota_code == START_CODE )
			{
				if(length == 0)
				{
					flash_unlock();
					flash_erase_N_pages(START_ADDRESS, 50);
					flash_lock();
				}
				ota_send_respond(ACK);
				ota_state = OTA_SEND_INFOR_STATE;
			}
		}
      break;

    case OTA_SEND_INFOR_STATE:
		{
			OTAInfor *infor = (OTAInfor*)min_payload;
			if(infor->command_id == OTA_INFOR)
			{
				strcpy(name,(char*)infor->name);
				strcpy(version, (char*)infor->version);
				ota_send_respond(ACK);
				ota_state = OTA_SEND_DATA_STATE;
			}
		}
      break;

    case OTA_SEND_DATA_STATE:
		{
			uint8_t cmd = min_payload[0];

			if(cmd == OTA_DATA)
			{
        OTAData *data = (OTAData*)min_payload;
				flash_unlock();
				flash_write_arr(START_ADDRESS + length, data->data, data->len);
				flash_lock();
				length += data->len;
			}
			else if(cmd == OTA_CODE)
			{
        OTACode *ota_code = (OTACode*)min_payload;
        if(ota_code->ota_code == END_CODE)
        {
          ota_send_respond(ACK);
          ota_state = OTA_IDLE_STATE;
					length = 0;
					update_state = DONE_UPDATE;
        }
			}
		}
      break;
		
		case OTA_END_STATE:
			break;
		
    default:
      break;
	}
}

OTA_State_Update_t is_updated(void)
{
	return update_state;
}

void bootloader_handle(void)
{
	uint8_t data;
	uint8_t len = 0;
	if(UART_Available() > 0)
	{
		data = UART_Read();
		len = 1;
	}
	min_poll(&min_ctx, &data, len);
}

void bootloader_init(void)
{
	UART_Init();
	min_init_context(&min_ctx, MIN_PORT);
	ota_state = OTA_IDLE_STATE;
}
