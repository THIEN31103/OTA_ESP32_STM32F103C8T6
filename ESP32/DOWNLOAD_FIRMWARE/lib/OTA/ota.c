#include"ota.h"
#define KEY_NAME    "NAME"
#define KEY_VERSION "VERSION"

static Typedef_OTA_Status_t ota_status;
static char name[20];
static char version[10];
static char old_name[20];
static char old_version[10];

void OTA_init(const char *web_server_init, const char *web_port_init)
{
    download_init(web_server_init, web_port_init);
    bootloader_init();
}

Typedef_OTA_Status_t OTA_status(void)
{
    download_status_ota(&ota_status);
    return ota_status;
}

void OTA_start(void)
{
    while(1)
    {
        if(OTA_status() == OTA)
        {
            uint8_t ota = 0;
            flash_app_nvs_get_str(KEY_NAME, old_name);
            flash_app_nvs_get_str(KEY_VERSION, old_version);
        
            download_version(name, version);
            if(strcmp(name, old_name) != 0)
            {
                flash_app_nvs_set_str(KEY_NAME, name);
                printf("UPDATE NAME: %s -> %s\n", old_name, name);
                ota = 1;
            }

            if(strcmp(version, old_version) != 0)
            {
                flash_app_nvs_set_str(KEY_VERSION, version);
                printf("UPDATE VERSION: %s -> %s\n", old_version, version);
                ota = 1;
            }
            if(ota)
            {
                bootloader_send_data(&ota, 1);
            printf("OTA = %d\n", ota);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}