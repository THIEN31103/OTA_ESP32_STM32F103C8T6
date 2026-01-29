#include"flash_app.h"

#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#define USER_NAMESPACE  "app"
static const char *TAG = "FLASH";
static nvs_handle_t my_handle;

void flash_app_nvs_set_value(char *key, int value)
{
    int err;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"NVS OPEN FAIL");
    }
    err = nvs_set_i32(my_handle, key, value);
    err = nvs_commit(my_handle);
}

void flash_app_nvs_set_str(char *key, char *str)
{
    int err;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"NVS OPEN FAIL");
    }
    err = nvs_set_str(my_handle, key, str);

    err = nvs_commit(my_handle);
}

void flash_app_nvs_get_value(char *key, void *value)
{
    int32_t *tmp = (int32_t*)value;
    int err;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"NVS OPEN FAIL");
    } 
    err = nvs_get_i32(my_handle, key, tmp);
    switch (err) {
        case ESP_OK:
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG,"The value is not initialized yet!");
            break;

        default :
            ESP_LOGE(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }
}

void flash_app_nvs_get_str(char *key, char *str)
{
    int err;
    size_t length = 0;
    err = nvs_open(USER_NAMESPACE, NVS_READWRITE, &my_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"NVS OPEN FAIL");
    } 
    err = nvs_get_str(my_handle, key, str, &length);
    err = 0;
    err = nvs_get_str(my_handle, key, str, &length);
    switch (err) {
        case ESP_OK:
            break;

        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGI(TAG,"The value is not initialized yet!");
            break;
            
        default :
            ESP_LOGE(TAG,"Error (%s) reading!\n", esp_err_to_name(err));
        }
}
