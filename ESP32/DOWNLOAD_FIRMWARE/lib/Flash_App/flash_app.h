#ifndef __FLASH_APP_H__
#define __FLASH_APP_H__
#include <stdint.h>

void flash_app_nvs_set_value(char *key, int value);
void flash_app_nvs_set_str(char *key, char *str);

void flash_app_nvs_get_value(char *key, void *value);
void flash_app_nvs_get_str(char *key, char *str);
#endif
