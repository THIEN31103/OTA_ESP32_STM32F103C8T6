#include"wifi_config.h"
#include "esp_mac.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "esp_smartconfig.h"
#include "esp_log.h"

#include "http_server_app.h"

static const char *TAG = "CONFIG_WIFI";

static provison_type_t provison_tye = PROVISON_ACCESSPONIT;
static EventGroupHandle_t s_wifi_event_group;
static const int WIFI_CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int HTTP_DONE_BIT = BIT2;

static char ssid[50];
static char pass[50]; 

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) 
    {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } 

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) 
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED)
     {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }

    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) 
    {
        ESP_LOGI(TAG, "Scan done");
    }
     
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) 
    {
        ESP_LOGI(TAG, "Found channel");
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) 
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = { 0 };
        uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));

        if (strlen((char*)evt->password) == 1) 
        {
            wifi_config.sta.password[0] = '\0';
            wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
        } 
        else 
        {
            memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:");
            for (int i=0; i<33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK( esp_wifi_disconnect() );
        ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
        esp_wifi_connect();
    } 
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) 
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

/*Check information wiifi in flash*/
static bool is_provisioned(void)
{
    bool provisioned = false;

    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t ctg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&ctg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

    if(wifi_config.sta.ssid[0] != 0x00)
    {
        provisioned = true;
    }
    return provisioned;
}

static void ap_start()
{
     wifi_config_t wifi_config = {
        .ap = {
            .ssid = "DUCTHIEN",
            .ssid_len = strlen("DUCTHIEN"),
            .channel = 1,
            .password = "123456789",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    if (wifi_config.ap.password[0] == '\0') 
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void http_post_data_callback(char* data, int len)
{
    /* SSID@PASS@ */
    char* token = strtok(data, "@");
    if(token)
    {
        strcpy(ssid, token);
    }
    else
    {
        ESP_LOGI(TAG, "NOT FOUND SSID");
    }

    token = strtok(NULL,"@");

    if(token)
    {
        strcpy(pass, token);
    }
    else
    {
        pass[0] = '\0';
    }
    printf("SSID: %s\n", ssid);
    printf("LEN_PASS: %d\n", strlen(pass));
    xEventGroupSetBits(s_wifi_event_group, HTTP_DONE_BIT);
}

void app_config(void)
{
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    bool provisoned = is_provisioned();
    if(!provisoned)
    {
        if(provison_tye == PROVISON_SMARTCONFIG)
        {
            ESP_ERROR_CHECK(esp_wifi_start());
            ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_ESPTOUCH) );
            smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
            ESP_ERROR_CHECK( esp_smartconfig_start(&cfg) );
            
            xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
            esp_smartconfig_stop();
        }
        else if(provison_tye == PROVISON_ACCESSPONIT)
        {
            ap_start();
            start_webserver();
            http_set_callback_post(http_post_data_callback);
            xEventGroupWaitBits(s_wifi_event_group, HTTP_DONE_BIT, false, false, portMAX_DELAY);

            stop_webserver();

            wifi_config_t wifi_config;
            bzero(&wifi_config,sizeof(wifi_config));

            memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
            // memcpy(wifi_config.sta.password, pass, strlen(pass));

            if (strlen(pass) == 1) 
            {
                wifi_config.sta.password[0] = '\0';
                wifi_config.sta.threshold.authmode = WIFI_AUTH_OPEN;
            } 
            else 
            {
                memcpy(wifi_config.sta.password, pass, strlen(pass));
            }

            wifi_init_config_t ctg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&ctg));
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            ESP_ERROR_CHECK(esp_wifi_start());
        }
    }
    else
    {
        ESP_ERROR_CHECK(esp_wifi_start());
    }
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, false, false, portMAX_DELAY);
    ESP_LOGI(TAG,"WFIF CONNECTED");
}