set(app_src http_server_app.c)

set (pri_req esp_http_server 
            esp_netif 
            esp-tls 
            esp_wifi
            esp_eth)

idf_component_register(
    SRCS ${app_src}
    INCLUDE_DIRS "."
    REQUIRES ${pri_req}
)

target_add_binary_data(${COMPONENT_LIB} "${CMAKE_SOURCE_DIR}/html/index.html" TEXT)
