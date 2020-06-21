#ifndef WIFICONFIGSTATE_H
#define WIFICONFIGSTATE_H

#include "Context.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <WiFi.h>

class WifiConfigState: public State
{

public:
    static WifiConfigState *getInstance()
    {
        static WifiConfigState instance;
        return &instance;
    }

    void buttonPressedLong(Context *c);
    void pressing10Sec(Context *c);
    void setInfo(const char *ssid, const char *password);

    WiFiServer server;
    IPAddress ip;

    // WiFiClient client;
    // String header;
    const String ap_ssid = "PETTERN_KOTI";

    int scan_nbr;
    SemaphoreHandle_t _mutex = NULL;

    TaskHandle_t _setting_task_handle;
    TaskHandle_t _server_task_handle;
    TaskHandle_t _scan_task_handle;

private:
    WifiConfigState() {}

    void begin(Context *);
    void end(Context *);

};

#endif /* WIFICONFIGSTATE_H */
