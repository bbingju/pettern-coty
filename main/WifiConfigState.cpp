#include "WifiConfigState.h"
#include "NormalState.h"
#include "State.h"
#include "esp_log.h"

#include <ArduinoJson.h>
#include <Preferences.h>
Preferences prefs;

static const char *TAG = "wifi_state";

extern "C" {

    const int capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(15);
    static StaticJsonDocument<capacity> scanlist;
    static char json_wifilist[2048];

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
            s->scan_nbr = WiFi.scanNetworks();
            ESP_LOGI(TAG, "scan networks nbr: %d", s->scan_nbr);

            if (s->scan_nbr) {

                JsonArray list = scanlist.createNestedArray("wifi_list");

                if (s->scan_nbr > 15) {
                    s->scan_nbr = 15;
                }

                for (int i = 0; i < s->scan_nbr; i++) {
                    JsonObject item = list.createNestedObject();
                    item["ssid"] = WiFi.SSID(i).c_str();
                    item["is_protected"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? false : true;

                    ESP_LOGI(TAG, "\t%d: %s (%d) %c",
                             i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                             (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? ' ' : '*');
                }
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        }

    }

    static void wifi_server_task(void *arg)
    {
        Context *c = (Context *) arg;
        WifiConfigState *s = (WifiConfigState *) c->getState();

        WiFi.softAP(s->ssid);
        s->ip = WiFi.softAPIP();
        //Serial.print("AP IP address: ");
        //Serial.println(s->ip);

        s->server.begin();

        while (true) {

            WiFiClient client = s->server.available();
            if (!client) {
                vTaskDelay(pdMS_TO_TICKS(100));
                continue;
            }

            ESP_LOGI(TAG, "New Client.");
            String header;
            String currentLine = "";
            while (client.connected()) {
                if (client.available()) {
                    char c = client.read();
                    printf("%c", c);
                    header += c;
                    if (c == '\n') { // if the byte is a newline character
                        // if the current line is blank, you got two newline characters in a row.
                        // that's the end of the client HTTP request, so send a response:

                        if (header.indexOf("GET /api/routers") >= 0) {
                            if (currentLine.length() == 0) {
                                int scan_nbr = WiFi.scanNetworks();
                                ESP_LOGI(TAG, "scan networks nbr: %d", scan_nbr);

                                if (scan_nbr) {
                                    JsonObject root = scanlist.to<JsonObject>();
                                    JsonArray list = root.createNestedArray("wifi_list");

                                    if (scan_nbr > 15) {
                                        scan_nbr = 15;
                                    }

                                    json_wifilist[0] = '\0';
                                    strcpy(json_wifilist, "{\"wifi_list\": [");

                                    for (int i = 0; i < scan_nbr; i++) {
                                        char json_item[256] = { 0 };
                                        json_item[0] = '{';
                                        sprintf(json_item, "{\"ssid\": \"%s\", \"is_protected\": %s},",
                                                WiFi.SSID(i).c_str(),
                                                (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "false" : "true");
                                        strcat(json_wifilist, json_item);

                                        // JsonObject item = list.createNestedObject();
                                        // item["ssid"] = WiFi.SSID(i);
                                        // item["is_protected"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? false : true;

                                        // ESP_LOGI(TAG, "\t%d: %s (%d) %c",
                                        //          i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i),
                                        //          (WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?' ': '*');
                                    }
                                    json_wifilist[strlen(json_wifilist) - 1] = '\0';
                                    strcat(json_wifilist, "]}\r\n\r\n");
                                    printf("%s", json_wifilist);
                                    // String output;
                                    // serializeJsonPretty(scanlist, output);
                                    // printf("%s\r\n", output.c_str());
                                }

                                // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                // and a content-type so the client knows what's coming, then a blank line:
                                if (scan_nbr) {
                                    client.println(F("HTTP/1.1 200 OK"));
                                    client.println(F("Content-type: application/json"));
                                    client.println(F("Connection: close"));
                                    client.print(F("Content-Length: "));
                                    client.println(strlen(json_wifilist));
                                    // client.println(measureJson(scanlist));
                                    client.println();
                                    client.println(json_wifilist);
                                    // serializeJson(scanlist, client);
                                    //client.println();
                                } else {
                                    client.println(F("HTTP/1.1 400 Bad Request"));
                                    client.println(F("Connection: close"));
                                    client.println();
                                }
                            } else {
                                currentLine = "";
                            }
                        } else if (c != '\r') {
                            currentLine += c;
                        }
                    }
                } else if (header.indexOf("POST /api/routers") >= 0) {
                    // s->setInfo("test", "11112222");
                }
            }
            header = "";
            client.stop();
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
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

    xTaskCreatePinnedToCore(wifi_setting_task, "wifi_setting",
                            1024, c, 3, &_setting_task_handle, 0);

    xTaskCreatePinnedToCore(wifi_server_task, "wifi_server",
                            4096, c, 3, &_server_task_handle, 0);

    //    xTaskCreatePinnedToCore(wifi_scan_task, "wifi_scan",
    //                            4096, c, 3, &_scan_task_handle, 0);
}

void WifiConfigState::end(Context *c)
{
    c->printStringToLED("    ");
    c->changeButtonColor(LED::BLACK);

    server.close();
    //    vTaskDelete(_scan_task_handle);
    vTaskDelete(_server_task_handle);
    vTaskDelete(_setting_task_handle);
}

void WifiConfigState::setInfo(const char *ssid, const char *password)
{
    prefs.begin("wifi_info");
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.end();

    ESP_LOGI(TAG, "%s: ssid:%s, password:%s", __func__, ssid, password);
}
