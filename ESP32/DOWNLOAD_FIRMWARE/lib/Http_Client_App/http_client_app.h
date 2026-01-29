#ifndef __HTTP_CLIENT_APP__
#define __HTTP_CLIENT_APP__
#include"stdint.h"

typedef void (*http_client_get_callback_t) (char *data, uint32_t len);

void http_client_init(const char *web_server_init, const char *web_port_init);
void http_client_get_firmware(char *request, char *recv_buf, char *get);
void http_client_get_version(char *request, char *recv_buf, char *get);
void http_client_get_status_ota(char *request, char *recv_buf, char *get);
void http_client_get_firmware_set_callback(void *cb);
void http_client_get_version_set_callback(void *cb);
void http_client_get_status_ota_set_callback(void *cb);

#endif