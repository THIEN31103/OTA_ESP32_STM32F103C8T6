#ifndef __DOWNLOAD_H__
#define __DOWNLOAD_H__
#include"stdint.h"
#include"stdbool.h"

typedef enum
{
    DOWNLOAD_OK,
    DOWNLOAD_ERROR,
}Typedef_Download_Status_t;

typedef enum
{
    NO_OTA,
    OTA,
}Typedef_OTA_Status_t;

void download_init(const char *web_server_init, const char *web_port_init);
uint8_t download_version(char *name, char *version);
uint8_t download_and_send_firmware(void);
uint8_t download_status_ota(Typedef_OTA_Status_t *status_ota);
#endif