#ifndef ASYNC_WEB_SERVER_H_
#define ASYNC_SERVER_H_
#include <ESPAsyncWebServer.h>

extern AsyncWebServer* server;

void server_init();
void refresh_sensor_data();
void remote_start_check();

#endif