#include"bootloader_app.h"
#include"min.h"
#include"app_uart.h"
#include"download.h"

#define MIN_ID 1
struct min_context min_ctx;

OTA_State_Typedef ota_state;

static char name[20];
static char version[10];

void rx_callack(uint8_t *data, uint32_t len)
{
    min_poll(&min_ctx, data, len);
}

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

void ota_send_infor(char *name, char *version)
{
  OTAInfor infor;
  infor.command_id = OTA_INFOR;
  infor.len = sizeof(infor.name) + sizeof(infor.version);
  strcpy((char*)&infor.name, name);
  strcpy((char*)&infor.version, version);
  bootloader_send_data(&infor, sizeof(infor));
}

void ota_send_data(uint8_t *data, uint8_t len)
{
  OTAData cmd;
  cmd.command_id = OTA_DATA;
  cmd.len = len;
  mempcpy(&cmd.data, data, len);
  bootloader_send_data(&cmd, sizeof(cmd));
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
          printf("SEND REQUEST\n");
          ota_send_code(REQUEST_CODE);
          printf("DONE SEND REQUEST\n");
          ota_state = OTA_START_STATE;
        }
        else
        {
          ota_state = OTA_START_STATE;
        }
      }
        break;

      case OTA_START_STATE:
      {
        OTARespond *respond = (OTARespond*)min_payload;
        if(respond->command_id == OTA_RESPOND && respond->ack == ACK)
        {
          printf("RECIVE ACK\n");
          printf("SEND START CODE\n");
          ota_send_code(START_CODE);
          printf("DONE SEND START CODE\n");
          ota_state = OTA_SEND_INFOR_STATE;
        }
        else
        {
          ota_state = OTA_IDLE_STATE;
        }
      }
        break;

      case OTA_SEND_INFOR_STATE:
      {
        OTARespond *respond = (OTARespond*)min_payload;
        if(respond->command_id == OTA_RESPOND && respond->ack == ACK)
        {
          printf("RECIVE ACK\n");
          printf("SEND INFOR\n");
          if(download_version(name, version) == DOWNLOAD_OK)
          {
            ota_send_infor(name, version);
            printf("DONE SEND INFOR\n");
            ota_state = OTA_SEND_DATA_STATE;
          }
          else
          {
            printf("ERROR SEND INFOR\n");
            ota_state = OTA_IDLE_STATE;
          }
        }
        else
        {
          ota_state = OTA_IDLE_STATE;
        }
      }
        break;

      case OTA_SEND_DATA_STATE:
      {
        OTARespond *respond = (OTARespond*)min_payload;
        if(respond->command_id == OTA_RESPOND && respond->ack == ACK)
        {
          printf("RECIVE ACK\n");
          printf("SEND DATA\n");
          if(download_and_send_firmware() == DOWNLOAD_OK)
          {
            printf("DONE SEND DATA\n");
            ota_send_code(END_CODE);
            ota_state = OTA_END_STATE;
          }
          else
          {
            printf("ERROR SEND DATA\n");
            ota_state = OTA_IDLE_STATE;
          }
        }
        else
        {
          ota_state = OTA_IDLE_STATE;
        }
      }
        break;
      
      case OTA_END_STATE:
      {
        OTARespond *respond = (OTARespond*)min_payload;
        if(respond->command_id == OTA_RESPOND && respond->ack == ACK)
        {
          printf("DONE UPDATE OTA\n");
          ota_state = OTA_IDLE_STATE;
        }
      }
        break;
      
      default:
        break;

    }
}
void bootloader_init()
{
  min_init_context(&min_ctx,MIN_ID);
  uart_rcv_set_callback(rx_callack);
  app_uart_start(2,17, 16, 115200);
  ota_state = OTA_IDLE_STATE;
}