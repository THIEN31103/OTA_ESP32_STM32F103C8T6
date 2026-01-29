#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "wifi_config.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"
#include "bootloader_app.h"
#include"flash_app.h"
#include"download.h"
#include"ota.h"

static const  char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    app_config();

    OTA_init("192.168.1.9","8000");
    OTA_start();
}