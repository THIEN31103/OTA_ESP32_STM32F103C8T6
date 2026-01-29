#ifndef __HTTP_SERVER_APP_H__
#define __HTTP_SERVER_APP_H__
#include<stdint.h>

typedef void(*http_post_callback_t) (char* data, int len);
typedef void(*http_get_callback_t) (void);

void start_webserver(void);
void stop_webserver(void);

void http_set_callback_get(void *cb);
void http_set_callback_post(void *cb);
void get_data_response(char *data, int len);
#endif
