#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "sdkconfig.h"
#include "http_client_app.h"

#define MAX_IP_LEN    32
#define MAX_PORT_LEN  8

char WEB_SERVER[MAX_IP_LEN];
char WEB_PORT[MAX_PORT_LEN];


static const char *TAG = "HTTP_CLIENT";
static http_client_get_callback_t http_client_get_version_callback = NULL;
static http_client_get_callback_t http_client_get_firmware_callback = NULL;
static http_client_get_callback_t http_client_get_status_ota_callback = NULL;

void http_client_get_firmware(char *request, char *recv_buf, char *get)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL)
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return;
    }

    /* Code to print the resolved IP.
    Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) 
    {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return;
    }
    ESP_LOGI(TAG, "Allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) 
    {
        ESP_LOGE(TAG, "Socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return;
    }

    ESP_LOGI(TAG, "Connected");
    freeaddrinfo(res);

    sprintf(request, get, WEB_SERVER);

    if (write(s, request, strlen(request)) < 0) 
    {
        ESP_LOGE(TAG, "Socket send failed");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) 
    {
        ESP_LOGE(TAG, "Failed to set socket receiving timeout");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Set socket receiving timeout success");

    /* Read HTTP response */

    char *resp = NULL;
    int total_len = 0;

    do
    {
        r = read(s, recv_buf, 512);
        if (r > 0)
        {
            char *new_buf = realloc(resp, total_len + r + 1);
            if (!new_buf) {
                free(resp);
                ESP_LOGE(TAG, "Out of memory");
                break;
            }

            resp = new_buf;
            memcpy(resp + total_len, recv_buf, r);
            total_len += r;
            resp[total_len] = '\0';
        }  
    }
    while (r > 0);
    http_client_get_firmware_callback(resp, total_len);
    free(resp);
    close(s);
    ESP_LOGI(TAG, "Done read response HTTP!");
}

void http_client_get_version(char *request, char *recv_buf, char *get)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL)
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return;
    }

    /* Code to print the resolved IP.
    Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) 
    {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return;
    }
    ESP_LOGI(TAG, "Allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) 
    {
        ESP_LOGE(TAG, "Socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return;
    }

    ESP_LOGI(TAG, "Connected");
    freeaddrinfo(res);

    sprintf(request, get, WEB_SERVER);

    if (write(s, request, strlen(request)) < 0) 
    {
        ESP_LOGE(TAG, "Socket send failed");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) 
    {
        ESP_LOGE(TAG, "Failed to set socket receiving timeout");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Set socket receiving timeout success");

    /* Read HTTP response */

    char *resp = NULL;
    int total_len = 0;

    do
    {
        r = read(s, recv_buf, 512);
        if (r > 0)
        {
            char *new_buf = realloc(resp, total_len + r + 1);
            if (!new_buf) {
                free(resp);
                ESP_LOGE(TAG, "Out of memory");
                break;
            }

            resp = new_buf;
            memcpy(resp + total_len, recv_buf, r);
            total_len += r;
            resp[total_len] = '\0';
        }  
    }
    while (r > 0);
    http_client_get_version_callback(resp, total_len);
    free(resp);
    close(s);
    ESP_LOGI(TAG, "Done read response HTTP!");
}

void http_client_get_status_ota(char *request, char *recv_buf, char *get)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL)
    {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        return;
    }

    /* Code to print the resolved IP.
    Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) 
    {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return;
    }
    ESP_LOGI(TAG, "Allocated socket");

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) 
    {
        ESP_LOGE(TAG, "Socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        return;
    }

    ESP_LOGI(TAG, "Connected");
    freeaddrinfo(res);

    sprintf(request, get, WEB_SERVER);

    if (write(s, request, strlen(request)) < 0) 
    {
        ESP_LOGE(TAG, "Socket send failed");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Socket send success");

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) 
    {
        ESP_LOGE(TAG, "Failed to set socket receiving timeout");
        close(s);
        return;
    }
    ESP_LOGI(TAG, "Set socket receiving timeout success");

    /* Read HTTP response */

    char *resp = NULL;
    int total_len = 0;

    do
    {
        r = read(s, recv_buf, 512);
        if (r > 0)
        {
            char *new_buf = realloc(resp, total_len + r + 1);
            if (!new_buf) {
                free(resp);
                ESP_LOGE(TAG, "Out of memory");
                break;
            }

            resp = new_buf;
            memcpy(resp + total_len, recv_buf, r);
            total_len += r;
            resp[total_len] = '\0';
        }  
    }
    while (r > 0);
    http_client_get_status_ota_callback(resp, total_len);
    free(resp);
    close(s);
    ESP_LOGI(TAG, "Done read response HTTP!");
}

void http_client_init(const char *web_server_init, const char *web_port_init)
{
    strncpy(WEB_SERVER, web_server_init, sizeof(WEB_SERVER) - 1);
    WEB_SERVER[sizeof(WEB_SERVER) - 1] = '\0';

    strncpy(WEB_PORT, web_port_init, sizeof(WEB_PORT) - 1);
    WEB_PORT[sizeof(WEB_PORT) - 1] = '\0';

    ESP_LOGI(TAG, "Init OK: server=%s port=%s", WEB_SERVER, WEB_PORT);
}

void http_client_get_firmware_set_callback(void *cb)
{
    http_client_get_firmware_callback = cb;
}

void http_client_get_version_set_callback(void *cb)
{
    http_client_get_version_callback = cb;
}

void http_client_get_status_ota_set_callback(void *cb)
{
    http_client_get_status_ota_callback = cb;
}
