#include"download.h"
#include "http_client_app.h"
#include "cJSON.h"
#include "process_file_hex.h"
#include "string.h"
#include "stdbool.h"
#include"bootloader_app.h"

static char request[512];
static char recv_buf[512];
static char get_firmware[] = "GET /firmware HTTP/1.1\r\n""Host: %s\r\n""Connection: close\r\n""\r\n";
static char get_version[] = "GET /version HTTP/1.1\r\n""Host: %s\r\n""Connection: close\r\n""\r\n";
static char get_status_ota[] = "GET /status-ota HTTP/1.1\r\n""Host: %s\r\n""Connection: close\r\n""\r\n";

static char _name[20];
static char _version[10];
static bool is_done_download_firmware = false;
static bool is_done_download_ota_status = false;
static Typedef_OTA_Status_t _status_ota;

static void get_version_http(char *data, uint32_t len, char *name, char *version)
{
    uint8_t cnt = 0;
    char *token = strtok(data, "\r\n");
    while (token != NULL)
    {
        cnt++;
        if(cnt == 8)
        {
            cJSON *root = cJSON_Parse(token);
            if (root == NULL) 
            {
                printf("JSON parse error\n");
                break;
            }
            cJSON *_name = cJSON_GetObjectItem(root, "name");
            cJSON *_version = cJSON_GetObjectItem(root, "version");
            
            if (cJSON_IsString(_name) && cJSON_IsString(_version)) 
            {
                strcpy(name, _name->valuestring);
                strcpy(version, _version->valuestring);
            }
            else
            {
                printf("Not found name and version\n");
            }
            break;
        }
        token = strtok(NULL,"\r\n");
    }   
}

static void http_client_get_version_callback(char *data, uint32_t len)
{
    get_version_http(data, len, _name, _version);
}

uint8_t download_version(char *name, char *version)
{
    http_client_get_version(request, recv_buf, get_version);
    if(_name[0] == '\0' || _version[0] == '\0')
        return DOWNLOAD_ERROR;
    strcpy(name, _name);
    strcpy(version, _version);
    return DOWNLOAD_OK;
}

static void get_file_hex_http(char *data, uint32_t len)
{
    uint16_t cnt = 0;
    uint16_t line = 0;
    char *token = strtok(data, "\r\n");
    while (token != NULL)
    {
        cnt++;
        if(cnt >= 12)
        {
            line++;
            uint8_t hex_data[21];
            convert_string_to_array_hex(token, hex_data);
            if(check_sum(hex_data, hex_data[0] + 5) != CRC_OK)
                break;
            if(hex_data[3] == 0x00)
            {
                printf("LINE %d: ", line);   
                print_data(&hex_data[4], hex_data[0]);
                ota_send_data(&hex_data[4], hex_data[0]);
                line++;
            } 
        }
        token = strtok(NULL,"\r\n");
    }
    is_done_download_firmware = true;
}

static void http_client_get_firmware_callback(char *data, uint32_t len)
{
    get_file_hex_http(data, len);
}

uint8_t download_and_send_firmware(void)
{
    http_client_get_firmware(request, recv_buf, get_firmware);
    if(is_done_download_firmware)
        return DOWNLOAD_OK;
    return DOWNLOAD_ERROR;
}

static void get_status_ota_http(char *data, uint32_t len, Typedef_OTA_Status_t *status_ota)
{
    uint8_t cnt = 0;
    char *token = strtok(data, "\r\n");
    while (token != NULL)
    {
        cnt++;
        if(cnt == 8)
        {
            if(strstr(token, "\"ota\":true"))
            {
                *status_ota = OTA;
                break;
            }
            else if (strstr(token,"\"ota\":false"))
            {
                *status_ota = NO_OTA;
                break;
            }
            else
            {
                break;
            }
        }
        token = strtok(NULL,"\r\n");
    } 
    is_done_download_ota_status = true;  
}

static void http_client_get_status_ota_callback(char *data, uint32_t len)
{
    get_status_ota_http(data, len, &_status_ota);
}

uint8_t download_status_ota(Typedef_OTA_Status_t *status_ota)
{
    http_client_get_status_ota(request, recv_buf, get_status_ota);
    *status_ota = _status_ota;
    if(is_done_download_ota_status)
        return DOWNLOAD_OK;
    return DOWNLOAD_ERROR;
}

void download_init(const char *web_server_init, const char *web_port_init)
{
    http_client_get_firmware_set_callback(http_client_get_firmware_callback);
    http_client_get_version_set_callback(http_client_get_version_callback);
    http_client_get_status_ota_set_callback(http_client_get_status_ota_callback);
    http_client_init(web_server_init,web_port_init);
}
