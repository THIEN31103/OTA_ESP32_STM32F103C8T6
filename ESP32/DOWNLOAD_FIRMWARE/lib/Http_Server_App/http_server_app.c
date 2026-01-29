#include "http_server_app.h"

#include <string.h>
#include <stdlib.h>
#include <esp_log.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_tls_crypto.h"
#include <esp_http_server.h>
#include "esp_event.h"
#include "esp_tls.h"
#include "esp_check.h"

#include <esp_wifi.h>
#include <esp_system.h>
#include "esp_eth.h"

#define EXAMPLE_HTTP_QUERY_KEY_MAX_LEN  (64)

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;
static httpd_req_t *REG;

static http_get_callback_t http_get_callback = NULL;
static http_post_callback_t http_post_callback = NULL;

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");

void get_data_response(char *data, int len)
{
    httpd_resp_send(REG, data, len);
}

/* An HTTP GET handler */
static esp_err_t html_handler(httpd_req_t *req)
{
    // const char* resp_str = (const char*) "Hello World";
    // httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    size_t html_len = (size_t)(index_html_end - index_html_start);
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)index_html_start, html_len);
    return ESP_OK;
}

static const httpd_uri_t html = {
    .uri       = "/html",
    .method    = HTTP_GET,
    .handler   = html_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

static esp_err_t get_data_handler(httpd_req_t *req)
{
    // const char* resp_str = (const char*) "{\"temperature\": \"27.3\",\"humidity\": \"80\"}";
    // httpd_resp_send(req, resp_str, strlen(resp_str));
    REG = req; 
    http_get_callback();
    return ESP_OK;
}

static const httpd_uri_t get_data = {
    .uri       = "/get_data",
    .method    = HTTP_GET,
    .handler   = get_data_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

/* An HTTP Post handler */
static esp_err_t post_data_handler(httpd_req_t *req)
{
    char buf[100];
    /* Read the data for the request */
    httpd_req_recv(req, buf, req->content_len);
    http_post_callback(buf, req->content_len);

    /* End response*/
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t post_data = {
    .uri       = "/post_data",
    .method    = HTTP_POST,
    .handler   = post_data_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/data1", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/data1 URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &html);
        httpd_register_uri_handler(server, &get_data);
        httpd_register_uri_handler(server, &post_data);
        // httpd_register_uri_handler(server, &echo);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else
    {
        ESP_LOGI(TAG, "Error starting server!");
    }
}

void stop_webserver(void)
{
    /*Stop the httpd server */
    httpd_stop(server);
}

void http_set_callback_get(void *cb)
{
    http_get_callback = cb;
}

void http_set_callback_post(void *cb)
{
    http_post_callback = cb;
}