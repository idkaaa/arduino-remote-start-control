#ifndef ASYNC_WEB_SERVER_H_
#define ASYNC_SERVER_H_
#include <ESPAsyncWebServer.h>

AsyncWebServer* server;
bool IsRebootRequired;

void server_init();

#endif