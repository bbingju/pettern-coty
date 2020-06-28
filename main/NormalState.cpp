#include "NormalState.h"
#include "OffState.h"
#include "MeasureState.h"
#include "WifiConfigState.h"
#include <esp_task_wdt.h>
#include <esp_log.h>
#include "PID.h"

#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

static const char *TAG = "NORMAL_STATE";

#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     2

#define CHECK_ERROR_CODE(returned, expected) ({ \
            if(returned != expected){           \
                printf("TWDT ERROR\n");         \
                abort();                        \
            }                                   \
        })

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 2, Ki = 5, Kd = 1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

const char *mqtt_broker = "api.pettern.co.kr";

extern "C" void mqtt_callback(char *topic, byte *message, unsigned int length);

static WiFiClient wifi_client;
static PubSubClient mqtt_client(mqtt_broker, 1883, mqtt_callback, wifi_client);

static StaticJsonDocument<2048> doc;
HTTPClient http;

extern "C" void pid_task(void *arg)
{
    Context *c = (Context *)arg;

    Input = c->readCurrentTemperature();
    Setpoint = c->getTargetTemperature();

    myPID.SetMode(AUTOMATIC);

    while (1) {

        Input = c->readCurrentTemperature();
        Setpoint = c->getTargetTemperature();

        myPID.Compute();

        double current = c->readCurrentTemperature();
        double target = c->getTargetTemperature();
        float target_l = target - 0.5f;
        float target_h = target + 0.5f;

        if (c->isColdMode()) {
            if (current > target_h) {
                if (!c->isTecCooling()) {
                    c->tecCoolDown();
                    c->pumpOn();
                    c->fanOn();
                }
            } else if (current < target_l) {
                if (!c->isTecHeating()) {
                    c->tecHeatUp();
                    c->pumpOn();
                    c->fanOn();
                }
            } else {
                if (!c->isTecStopped()) {
                    c->tecStop();
                    c->pumpOff();
                    c->fanOff();
                }
            }
            //ESP_LOGI(TAG, "COLD MODE - Current: %.2f, Target: %.2f => %s",
            //         current, target, c->tecState());
        } else {
            if (current < target_l) {
                if (!c->isTecHeating()) {
                    c->tecHeatUp();
                    c->pumpOn();
                    c->fanOn();
                }
            } else if (current > target_h) {
                if (!c->isTecCooling()) {
                    c->tecCoolDown();
                    c->pumpOn();
                    c->fanOn();
                }
            } else {
                if (!c->isTecStopped()) {
                    c->tecStop();
                    c->pumpOff();
                    c->fanOff();
                }
            }
            ESP_LOGI(TAG, "HOT MODE - Current: %.2f, Target: %.2f => %s", current,
                     target, c->tecState());
        }

        if (c->isNormalState() && !c->isButtonValid()) {
            c->printTemperatureToLED(target);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

extern "C" void mqtt_callback(char *topic, byte *message, unsigned int length)
{
    ESP_LOGI(TAG, "Message arrived on topic %s.", topic);
    char *m = strndup((const char *) message, length);
    String msg = m;
    free(m);
    printf("Message: %s (%d)\r\n", msg.c_str(), length);

    Context *ctx = Context::getInstance();
    DeserializationError error = deserializeJson(doc, msg);

    if (error) {
        ESP_LOGI(TAG, "json error: %s", error.c_str());
        return;
    }

    int event_id = doc["event_id"];
    const char *action = doc["action"];
    char json_output[256] = { 0 };

    if (strcmp(action, "set_temperature") == 0) {

        const char *mode = doc["params"]["mode"];
        int temp = doc["params"]["value"];

	if (temp == 0) {	// old type
	  temp = doc["params"];
          mode = "cold";
	}
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
        //ESP_LOGI(TAG, "resp_url - %s", resp_url.c_str());

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
    } else if (strcmp(action, "get_device_status") == 0) {
        ESP_LOGI(TAG, "get_device_status received");

        const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(5);
        StaticJsonDocument<capacity> resp_data;
        JsonArray arr = resp_data.createNestedArray("data");
        JsonObject obj = arr.createNestedObject();
        Context *ctx = Context::getInstance();
        int current_temp = (int)ctx->readCurrentTemperature();

        obj["mode"] = ctx->isColdMode() ? "cool" : "hot";
        obj["temperature"] = current_temp < 0 ? abs(current_temp) : current_temp;
        obj["temperature_device"] = ctx->getTargetTemperature();
	obj["last_weight"] = (int) ctx->getLastWeight();

        serializeJson(resp_data, json_output, sizeof(json_output));
        //ESP_LOGI(TAG, "resp_data - %s", json_output);

        String resp_url = "http://api.pettern.co.kr/api/device_events/";
        resp_url.concat(event_id);
        resp_url.concat("/");
        //ESP_LOGI(TAG, "resp_url - %s", resp_url.c_str());

        if (WiFi.status() == WL_CONNECTED) {
            http.begin(resp_url);
            http.addHeader("Content-Type", "application/json");
            http.setUserAgent("Pettern-Coty");
            int http_resp_code = http.PUT(json_output);
            if (http_resp_code > 0) {
                String resp = http.getString();
                //ESP_LOGI(TAG, "%d: %s", http_resp_code, resp.c_str());
            } else {
                ESP_LOGI(TAG, "http req error %d", http_resp_code);
            }
            http.end();
        }
    }
}

extern "C" void mqtt_client_task(void *arg)
{
    Context *ctx = (Context *)arg;	

     if (mqtt_client.connect(ctx->getDeviceName().c_str())) {
        ESP_LOGI(TAG, "%s mqtt connected", ctx->getDeviceName().c_str());

	char *topic = NULL;
	asprintf(&topic, "devices/%s", ctx->getDeviceName().c_str());
        mqtt_client.subscribe(topic);
	free(topic);
    }

    mqtt_client.setServer(mqtt_broker, 1883);

    // CHECK_ERROR_CODE(esp_task_wdt_add(NULL), ESP_OK);
    // CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_OK);

    while (1) {
        // CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);
        mqtt_client.loop();
        // vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1));
    }
}

// extern void reconnect()
// {
//     // Loop until we're reconnected
//     while (!mqtt_client.connected()) {
//         Serial.print("Attempting MQTT connection...");
//         // Attempt to connect
//         if (mqtt_client.connect("Pettern-Coty Client")) {
//             Serial.println("connected");
//             // Subscribe
//             mqtt_client.subscribe("devices/1");
//         } else {
//             Serial.print("failed, rc=");
//             Serial.print(mqtt_client.state());
//             Serial.println(" try again in 5 seconds");
//             // Wait 5 seconds before retrying
//             delay(5000);
//         }
//     }
// }

void NormalState::buttonPressedShort(Context *c)
{
    c->stackState(MeasureState::getInstance());
}

void NormalState::buttonPressedLong(Context *c)
{
    int duration = c->buttonPressingDuration();

    ESP_LOGI(TAG, "duration: %d\n", duration);

    if (duration >= 6 && duration < 10) {
        c->changeState(OffState::getInstance());
    } else if (duration >= 10 /* && duration < 14 */) {
        c->changeState(WifiConfigState::getInstance());
    }
}

void NormalState::buttonPressedDouble(Context *c)
{
    app_event_emit(APP_EVENT_TEMP_MODE_TOGGLE, NULL);
}

void NormalState::pressing(Context *c)
{
    // static bool flag = true;
    // static int count = 0;

    // int duration = c->buttonPressingDuration();
    // if (duration >= 2 && duration < 4) {

    //      if (count == 0) {
    //      c->printStringToLED(flag ? (c->isColdMode() ? "HOT " : "COLD") : "    ");
    //  flag = !flag;
    //   }

    // } else if (duration >= 6 && duration < 10) {
    // if (count == 0) {
    //     c->printStringToLED(flag ? "BYE " : "    ");
    //     flag = !flag;
    // }
    // } else if (duration >= 10 && duration < 12) {
    // if (count == 0) {
    //     c->printStringToLED(flag ? "WIFI" : "    ");
    //     flag = !flag;
    // }
    // }

    // ++count;
    // count %= 3;
}

void NormalState::buttonPressingPerSec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    int duration = c->buttonPressingDuration();
    if (duration >= 2 && duration < 4) {

        if (count == 0) {
            c->printStringToLED(flag ? (c->isColdMode() ? "HOT " : "COLD") : "    ");
            flag = !flag;
        }

    } else if (duration >= 6 && duration < 10) {
        if (count == 0) {
            c->printStringToLED(flag ? "BYE " : "    ");
            flag = !flag;
        }
    } else if (duration >= 10 && duration < 12) {
        if (count == 0) {
            c->printStringToLED(flag ? "WIFI" : "    ");
            flag = !flag;
        }
    }

    ++count;
    count %= 3;
}

// void NormalState::pressing2Sec(Context *c)
// {
//     static bool flag = true;
//     static int count = 0;

//     if (count == 0) {
//         c->printStringToLED(flag ? (c->isColdMode() ? "HOT " : "COLD") : "    ");
//         flag = !flag;
//     }

//     ++count;
//     count %= 3;
// }

// void NormalState::pressing6Sec(Context *c)
// {
//     static bool flag = true;
//     static int count = 0;

//     if (count == 0) {
//         c->printStringToLED(flag ? "BYE " : "    ");
//         flag = !flag;
//     }

//     ++count;
//     count %= 3;
// }

// void NormalState::pressing10Sec(Context *c)
// {
//     static bool flag = true;
//     static int count = 0;

//     if (count == 0) {
//         c->printStringToLED(flag ? "WIFI" : "    ");
//         flag = !flag;
//     }

//     ++count;
//     count %= 3;
// }

void NormalState::begin(Context *ctx)
{
    ctx->changeButtonColor(ctx->isColdMode() ? LED::BLUE : LED::RED);

    xTaskCreatePinnedToCore(pid_task, "pid_task",
                            4096, ctx, 3, &_task_handle, 1);


    xTaskCreatePinnedToCore(mqtt_client_task, "mqtt_client_task",
                            4096, ctx, 4, &_mqtt_client_task_handle, 0);

    mqtt_client.setServer(mqtt_broker, 1883);
}

void NormalState::end(Context *c)
{
    c->tecStop();
    c->pumpOff();
    c->fanOff();

    vTaskDelete(_mqtt_client_task_handle);
    vTaskDelete(_task_handle);
}
