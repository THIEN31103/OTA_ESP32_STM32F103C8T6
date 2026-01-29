#ifndef __OTA_H__
#define __OTA_H__
#include "bootloader_app.h"
#include"flash_app.h"
#include"download.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void OTA_init(const char *web_server_init, const char *web_port_init);
void OTA_start(void);
Typedef_OTA_Status_t OTA_status(void);

#endif