set(app_src wifi_config.c)

set (pri_req esp_http_server 
            esp_netif 
            esp-tls 
            esp_wifi
            esp_eth
            nvs_flash 
            Http_Server_App)

idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)
