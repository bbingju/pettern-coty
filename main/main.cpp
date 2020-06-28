#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "Arduino.h"
#include "Context.h"
#include "app_event.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     2

#define CHECK_ERROR_CODE(returned, expected) ({ \
            if(returned != expected){           \
                printf("TWDT ERROR\n");         \
                abort();                        \
            }                                   \
        })

static const char *TAG = "main";

void setup_wifi(Context *ctx)
{
    WiFi.begin(ctx->getWiFiSSID().c_str(), ctx->getWiFiPassword().c_str());

    delay(500);
    ESP_LOGW(TAG, "WiFi.status() = %d", WiFi.status());
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500);
    //     // ESP_LOGI(TAG, ".");
    // }
}


extern "C" void app_main()
{
    initArduino();
    Context *ctx = Context::getInstance();
    setup_wifi(ctx);
    app_event_loop_create();
}   //
