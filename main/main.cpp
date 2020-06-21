#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
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

const char* ssid = "Wireless";
const char* password = "redphil!";
// const char* ssid = "NERDVANA-GUEST";
// const char* password = "11112222";
const char* mqtt_broker = "api.pettern.co.kr";
static StaticJsonDocument<2048> doc;
HTTPClient http;

void mqtt_callback(char* topic, byte* message, unsigned int length);

WiFiClient wifi_client;
PubSubClient mqtt_client(mqtt_broker, 1883, mqtt_callback, wifi_client);


void setup_wifi() {
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        // ESP_LOGI(TAG, ".");
    }
}


void mqtt_callback(char* topic, byte* message, unsigned int length) {
    ESP_LOGI(TAG, "Message arrived on topic %s.", topic);
    ESP_LOGI(TAG, "Message: %s (%d)", message, length);

    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        ESP_LOGI(TAG, "json error: %s", error.c_str());
        return;
    }

    int event_id = doc["event_id"];
    const char *action = doc["action"];
    char json_output[256] = { 0 };

    if (strcmp(action, "set_temperature") == 0) {
	Context *ctx = Context::getInstance();
        const char *mode = doc["params"]["mode"];
    	int temp = doc["params"]["value"];
        ESP_LOGI(TAG, "set_temperature: %d", temp);

	if (strcmp(mode, "cold") == 0) {
	  ctx->setTargetTemperature(TempMode::COLD, (float)temp);
	} else if (strcmp(mode, "warm") == 0 || strcmp(mode, "hot") == 0) {
		ctx->setTargetTemperature(TempMode::HOT, (float)temp);
	} else {
	  return;
	}

	const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3);
        StaticJsonDocument<capacity> resp_data;
        JsonArray arr = resp_data.createNestedArray("data");
        JsonObject obj = arr.createNestedObject();
        obj["temperature"] = temp;
        obj["mode"] = mode;

        serializeJson(resp_data, json_output, sizeof(json_output));
        ESP_LOGI(TAG, "resp_data - %s", json_output);

        String resp_url = "http://api.pettern.co.kr/api/device_events/";
        resp_url.concat(event_id);
        resp_url.concat("/");
        ESP_LOGI(TAG, "resp_url - %s", resp_url.c_str());
 
        if (WiFi.status() == WL_CONNECTED) {
            http.begin(resp_url);
            http.addHeader("Content-Type", "application/json");
            http.setUserAgent("Pettern-Coty");
            int http_resp_code = http.PUT(json_output);
            if (http_resp_code > 0) {
                String resp = http.getString();
                ESP_LOGI(TAG, "%d: %s", http_resp_code, resp.c_str());
            } else {
                ESP_LOGI(TAG, "http req error %d", http_resp_code);
            }
            http.end();
        }
    } 
    else if (strcmp(action, "get_device_status") == 0) {
        ESP_LOGI(TAG, "get_device_status received");

        const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(4);
        StaticJsonDocument<capacity> resp_data;
        JsonArray arr = resp_data.createNestedArray("data");
        JsonObject obj = arr.createNestedObject();
	Context *ctx = Context::getInstance();
	int current_temp = (int)ctx->readCurrentTemperature();

        obj["mode"] = ctx->isColdMode() ? "cool" : "hot";
        obj["temperature"] = current_temp;
        obj["temperature_device"] = 34;

        serializeJson(resp_data, json_output, sizeof(json_output));
        ESP_LOGI(TAG, "resp_data - %s", json_output);

        String resp_url = "http://api.pettern.co.kr/api/device_events/";
        resp_url.concat(event_id);
        resp_url.concat("/");
        ESP_LOGI(TAG, "resp_url - %s", resp_url.c_str());

        if (WiFi.status() == WL_CONNECTED) {
            http.begin(resp_url);
            http.addHeader("Content-Type", "application/json");
            http.setUserAgent("Pettern-Coty");
            int http_resp_code = http.PUT(json_output);
            if (http_resp_code > 0) {
                String resp = http.getString();
                ESP_LOGI(TAG, "%d: %s", http_resp_code, resp.c_str());
            } else {
                ESP_LOGI(TAG, "http req error %d", http_resp_code);
            }
            http.end();
        }
    }
}

void reconnect() {
    // Loop until we're reconnected
    while (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (mqtt_client.connect("Pettern-Coty Client")) {
            Serial.println("connected");
            // Subscribe
            mqtt_client.subscribe("devices/1");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqtt_client_task(void *arg)
{
    CHECK_ERROR_CODE(esp_task_wdt_add(NULL), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_OK);

    while (1) {
        CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        mqtt_client.loop();
        vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1));
    }
}

static TaskHandle_t mqtt_client_task_handle;
extern "C" void app_main()
{
    initArduino();
    setup_wifi();

    if (mqtt_client.connect("1")) {
        ESP_LOGI(TAG, "mqtt connected");

        mqtt_client.subscribe("devices/1");
    }

    xTaskCreatePinnedToCore(mqtt_client_task, "mqtt task", 2048 * 2, NULL, 10, &mqtt_client_task_handle, 1);

    mqtt_client.setServer(mqtt_broker, 1883);
    //mqtt_client.setCallback(mqtt_callback);

    Context *ctx = Context::getInstance();
    app_event_loop_create();
}   // 
