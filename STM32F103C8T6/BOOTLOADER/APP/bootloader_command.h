#ifndef __BOOTLOADER_COMMAND_H__
#define __BOOTLOADER_COMMAND_H__
#include "stdint.h"

typedef enum
{
  OTA_CODE,
  OTA_INFOR,
  OTA_DATA,
  OTA_RESPOND,  
}CmdID;

typedef enum
{
  START_CODE,
  END_CODE,
  REQUEST_CODE,
	NOT_NEW_VERSION
}OTA_Code_Name;

typedef enum
{
  ACK,
  NACK
}OTA_Respond_Name;

typedef struct 
{
  uint8_t command_id;
  uint8_t len;
  uint8_t ota_code;
} __attribute__((packed))OTACode;

typedef struct
{
  uint8_t command_id;
  uint8_t len;
  uint8_t ack;
}__attribute__((packed))OTARespond;

typedef struct
{
  uint8_t command_id;
  uint8_t len;
  uint8_t name[50];
  uint8_t version[20];
}__attribute__((packed)) OTAInfor;

typedef struct
{
  uint8_t command_id;
  uint8_t len;
  uint8_t data[16];
}__attribute__((packed)) OTAData;

#endif
