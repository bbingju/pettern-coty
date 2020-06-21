#include "WifiConfigState.h"
#include "NormalState.h"
#include "State.h"
#include "esp_log.h"

#include <ArduinoJson.h>
#include <Preferences.h>
Preferences prefs;

static const char *TAG = "wifi_state";

extern "C" {

    const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(40);
    static StaticJsonDocument<capacity> scanlist;

    static void wifi_setting_task(void *arg)
    {
        Context *c = (Context *) arg;

        while (1) {
            c->printStringToLED("WIFI");

            c->changeButtonColor(LED::RED);
            vTaskDelay(pdMS_TO_TICKS(400));
            c->changeButtonColor(LED::BLUE);
            vTaskDelay(pdMS_TO_TICKS(400));
        }
    }

    static void wifi_scan_task(void *arg)
    {
        Context *c = (Context *) arg;
        WifiConfigState *s = (WifiConfigState *) c->getState();

        while (1) {
            if (xSemaphoreTake(s->_mutex, 10) == pdTRUE) {
                s->scan_nbr = WiFi.scanNetworks();
                ESP_LOGI(TAG, "scan networks nbr: %d", s->scan_nbr);

                if (s->scan_nbr) {
                    if (scanlist.size()) {
                        scanlist.clear();
                    }

                    JsonArray list = scanlist.createNestedArray("wifi_list");

                    if (s->scan_nbr > 10) {
                        s->scan_nbr = 10;
                    }

                    for (int i = 0; i < s->scan_nbr; i++) {
                        JsonObject item = list.createNestedObject();
                        item["ssid"] = WiFi.SSID(i);
                        item["is_protected"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? false : true;

                        // ESP_LOGI(TAG, "\t%d: %s (%d) %c",
                        //          i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                        //          (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? ' ' : '*');
                    }

                    // String wifilist;
                    // serializeJson(scanlist, wifilist);
                    // ESP_LOGI(TAG, "%s", wifilist.c_str());
                }
                xSemaphoreGive(s->_mutex);
            }

            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }

    static void wifi_server_task(void *arg)
    {
        Context *c = (Context *) arg;
        WifiConfigState *s = (WifiConfigState *) c->getState();
        String header = "";

        WiFi.softAP(s->ap_ssid.c_str());
        s->ip = WiFi.softAPIP();
        Serial.print("AP IP address: ");
        Serial.println(s->ip);

        s->server.begin();

        while (true) {

            WiFiClient client = s->server.available();
            if (!client) {
                delay(1);
                continue;
            }

            ESP_LOGI(TAG, "New Client.");

            String currentLine = "";
            while (client.connected()) {

                if (client.available()) {
                    // String line = client.readStringUntil('\r');
                    // printf("%s", line.c_str());
                    char c = client.read();
                    putchar(c);
                    header += c;

                    if (c == '\n') {
                        if (currentLine.length() == 0) {
                            if (header.indexOf("GET /api/routers") >= 0) {
                                if (xSemaphoreTake(s->_mutex, 100) == pdTRUE && scanlist.size() > 0) {
                                    String wifilist;
                                    serializeJson(scanlist, wifilist);
                                    ESP_LOGI(TAG, "%s", wifilist.c_str());

                                    client.println(F("HTTP/1.1 200 OK"));
                                    client.println(F("Content-type: application/json"));
                                    client.println(F("Connection: close"));
                                    client.print(F("Content-Length: "));
                                    client.println(measureJson(scanlist));
                                    client.println();
                                    serializeJson(scanlist, client);

                                    xSemaphoreGive(s->_mutex);
                                } else {
                                    client.println(F("HTTP/1.1 400 Bad Request"));
                                    client.println(F("Connection: close"));
                                    client.println();
                                }
                                break;

                            } else if (header.indexOf("POST /api/routers/") >= 0) {
                                DynamicJsonDocument resp(1024);
                                resp["device_id"] = "1";

                                vTaskSuspend(s->_scan_task_handle);

                                String wifi_info = client.readString();
                                String ssid = wifi_info.substring(wifi_info.indexOf("ssid="));
                                String password = wifi_info.substring(wifi_info.indexOf("password="), wifi_info.indexOf("&"));
                                ssid = ssid.substring(ssid.indexOf("=") + 1);
                                password = password.substring(password.indexOf("=") + 1);
                                printf("ssid = %s, password = %s\r\n", ssid.c_str(), password.c_str());

                                client.println(F("HTTP/1.1 200 OK"));
                                client.println(F("Content-type: application/json"));
                                client.println(F("Connection: close"));
                                client.print(F("Content-Length: "));
                                client.println(measureJson(resp));
                                client.println();
                                serializeJson(resp, client);

                                String resp_json;
                                serializeJson(resp, resp_json);
                                printf("%s", resp_json.c_str());
                                vTaskResume(s->_scan_task_handle);
                                break;
                            } else {
                                client.println(F("HTTP/1.1 400 Bad Request"));
                                client.println(F("Connection: close"));
                                client.println();
                                break;
                            }

                        } else {
                            currentLine = "";
                        }
                    } else if (c != '\r') {  // if you got anything else but a carriage return character,
                        currentLine += c;      // add it to the end of the currentLine
                    }
                }
            } // end of client connected
            vTaskDelay(pdMS_TO_TICKS(100));
            // delay(1);
            header = "";
            client.stop();
            Serial.println("[Client disconnected]");
        } // end of while loop
    } // the end of function
}

void WifiConfigState::buttonPressedLong(Context *c)
{
    int duration = c->buttonPressingDuration();

    // ESP_LOGI(TAG, "duration: %d\n", duration);

    if (duration >= 4 && duration < 8) {
        c->changeState(NormalState::getInstance());
    }
}

void WifiConfigState::pressing10Sec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    if (count == 0) {
        c->printStringToLED(flag ? (c->isColdMode() ? "COLD" : "HOT") : "    ");
        flag = !flag;
    }

    ++count;
    count %= 3;
}

void WifiConfigState::begin(Context *c)
{
    c->printStringToLED("WIFI");
    c->changeButtonColor(LED::BLACK);

    vSemaphoreCreateBinary(_mutex);

    xTaskCreatePinnedToCore(wifi_setting_task, "wifi_setting",
                            1024, c, 3, &_setting_task_handle, 0);
    xTaskCreatePinnedToCore(wifi_server_task, "wifi_server",
                            4096, c, 3, &_server_task_handle, 0);
    xTaskCreatePinnedToCore(wifi_scan_task, "wifi_scan",
                            2048, c, 3, &_scan_task_handle, 0);
}

void WifiConfigState::end(Context *c)
{
    c->printStringToLED("    ");
    c->changeButtonColor(LED::BLACK);

    server.close();
    vTaskDelete(_scan_task_handle);
    vTaskDelete(_server_task_handle);
    vTaskDelete(_setting_task_handle);

    vSemaphoreDelete(_mutex);
}

void WifiConfigState::setInfo(const char *ssid, const char *password)
{
    prefs.begin("wifi_info");
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.end();

    ESP_LOGI(TAG, "%s: ssid:%s, password:%s", __func__, ssid, password);
}
