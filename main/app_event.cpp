#include "app_event.h"
#include <esp_log.h>
#include "Context.h"
#include "HX711.h"

const static char *TAG = "app_event";

esp_event_loop_handle_t app_loop;

/* Event source task related definitions */
ESP_EVENT_DEFINE_BASE(APP_EVENTS);

static const char* get_id_string(esp_event_base_t base, int32_t id)
{
    const char* event = "";

    if (base == APP_EVENTS) {
        switch (id) {
        case APP_EVENT_BTN_SHORT:
            event = "APP_EVENT_BTN_SHORT";
            break;
        case APP_EVENT_BTN_LONG_2SEC:
            event = "APP_EVENT_BTN_LONG_2SEC";
            break;
        case APP_EVENT_BTN_LONG_4SEC:
            event = "APP_EVENT_BTN_LONG_4SEC";
            break;
        case APP_EVENT_BTN_LONG_10SEC:
            event = "APP_EVENT_BTN_LONG_10SEC";
            break;
        case APP_EVENT_POWEROFF:
            event = "APP_EVENT_POWEROFF";
            break;
        case APP_EVENT_POWERON:
            event = "APP_EVENT_POWERON";
            break;
        case APP_EVENT_WEIGHT_MEASURE_START:
            event = "APP_EVENT_WEIGHT_MEASURE_START";
            break;
        case APP_EVENT_WEIGHT_MEASURE_STOP:
            event = "APP_EVENT_WEIGHT_MEASURE_STOP";
            break;
        case APP_EVENT_WIFI_CONFIG_START:
            event = "APP_EVENT_WIFI_CONFIG_START";
            break;
        case APP_EVENT_WIFI_CONFIG_STOP:
            event = "APP_EVENT_WIFI_CONFIG_STOP";
            break;
        case APP_EVENT_TEMP_MODE_TOGGLE:
            event = "APP_EVENT_TEMP_MODE_TOGGLE";
            break;
        default:
            event = "Unkown event";
        }
    }
    return event;
}

static void pid_task(void *arg)
{
    Context *c  = (Context *) arg;
    static char target_str[5] = {0};

    while (1) {

        float current = c->readCurrentTemperature();
        float target  = c->getTargetTemperature();
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
            ESP_LOGI(TAG, "COLD MODE - Current: %.2f, Target: %.2f => %s", current, target, c->tecState());
        } else {
            if (current < target_l) {
                if (!c->isTecHeating()) {
                    c->tecHeatUp();
                    c->pumpOn();
                    c->fanOn();
                }
            }  else if (current > target_h) {
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
            ESP_LOGI(TAG, "HOT MODE - Current: %.2f, Target: %.2f => %s", current, target, c->tecState());
        }

        if (c->isNormalState()) {
            snprintf(target_str, 4, "%.1f", target);
            c->printStringToLED(target_str);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void loadcell_task(void *arg)
{
    Context *c  = (Context *) arg;
    HX711 scale;
    static char weight_str[5] = { 0 };

    scale.begin(23, 22);

    // 영점조정
    scale.set_scale(293127.f); // this value is obtained by
                               // calibrating the scale with known
                               // weights; see the README for details
    scale.tare();              // reset the scale to 0

    // 영점조정 완료
    snprintf(weight_str, 4, "%.1f", 0.0f);
    c->printStringToLED("    ");
    delay(300);
    c->printStringToLED(weight_str);
    delay(300);
    c->printStringToLED("    ");
    delay(300);
    c->printStringToLED(weight_str);

    while (1) {

        long int v = scale.read_average(20);
        float w = (float) v;

        ESP_LOGI(TAG, "Loadcell - read average: %li", v);

        snprintf(weight_str, 4, "%.1f", w);
        c->printStringToLED(weight_str);

        scale.power_down();     // put the ADC in sleep mode
        delay(5000);
        scale.power_up();
    }
}

static void wifi_setting_task(void *arg)
{
    Context *c = (Context *) arg;

    while (1) {
        c->changeButtonColor(LED::RED);
        delay(400);
        c->changeButtonColor(LED::BLUE);
        delay(400);
    }
}

TaskHandle_t pid_task_handle;
TaskHandle_t loadcell_task_handle;
TaskHandle_t wifi_setting_task_handle;

static esp_timer_handle_t onpower_timer;
static esp_timer_create_args_t onpower_timer_args;

static void poweron_timer_callback(void* arg)
{
    Context *c = (Context *) arg;
    c->changeButtonColor(LED::BLACK);
}

static void poweroff_timer_callback(void* arg)
{
    Context *c = (Context *) arg;
    c->changeButtonColor(LED::BLACK);
    c->printStringToLED("    ");
}

static void handle_poweron()
{
    Context *c = Context::getInstance();

    c->printStringToLED(" ON ");
    delay(1200);

    if (c->isColdMode()) {
        c->changeButtonColor(LED::BLUE);
        c->printStringToLED("COLD");
    } else {
        c->changeButtonColor(LED::RED);
        c->printStringToLED("HOT ");
    }
    delay(500);

    xTaskCreatePinnedToCore(pid_task, "pid_task",
                            4096, c, 3, &pid_task_handle, 1);
}

static void handle_poweroff()
{
    Context *c = Context::getInstance();
    c->printStringToLED(" CU ");

    c->tecStop();
    c->pumpOff();
    c->fanOff();

    onpower_timer_args.callback = &poweroff_timer_callback;
    onpower_timer_args.arg = c;
    onpower_timer_args.name = "onpower_timer";
    esp_timer_create(&onpower_timer_args, &onpower_timer);
    esp_timer_start_once(onpower_timer, 1000 * 1000 * 2);

    vTaskDelete(pid_task_handle);
}

static void handle_weight_measure_start()
{
    Context *c = Context::getInstance();
    // c->printStringToLED("MSRT");
    xTaskCreatePinnedToCore(loadcell_task, "loadcell_task",
                            4096, c, 3, &loadcell_task_handle, 1);
}

static void handle_weight_measure_stop()
{
    Context *c = Context::getInstance();
    // c->printStringToLED("MSTP");
    vTaskDelete(loadcell_task_handle);
}

static void handle_wifi_config_start()
{
    Context *c = Context::getInstance();
    c->printStringToLED("WIFI");
    c->changeButtonColor(LED::BLACK);

    vTaskDelete(pid_task_handle);

    xTaskCreatePinnedToCore(wifi_setting_task, "wifi_task",
                            4096, c, 3, &wifi_setting_task_handle, 1);
}

static void handle_wifi_config_stop()
{
    Context *c = Context::getInstance();
    c->printStringToLED("    ");

    vTaskDelete(wifi_setting_task_handle);

    xTaskCreatePinnedToCore(pid_task, "pid_task",
                            4096, c, 3, &pid_task_handle, 1);
}

static void handle_temp_mode_toggle()
{
    Context *c = Context::getInstance();
    c->toggleTempMode();
    c->tecStop();
    if (c->isColdMode()) {
        c->changeButtonColor(LED::BLUE);
        c->printStringToLED("COLD");
    } else {
        c->changeButtonColor(LED::RED);
        c->printStringToLED("HOT ");
    }
    delay(500);
}

static void app_loop_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s", get_id_string(base, id));

    switch (id) {

    case APP_EVENT_BTN_SHORT: {
        // struct app_event_arg *d = (struct app_event_arg *) event_data;
        Context::getInstance()->pushShortKey();
        break;
    }
    case APP_EVENT_BTN_LONG_2SEC: {
        Context::getInstance()->pushLongKey2Sec();
        break;
    }
    case APP_EVENT_BTN_LONG_4SEC: {
        Context::getInstance()->pushLongKey4Sec();
        break;
    }
    case APP_EVENT_BTN_LONG_10SEC: {
        Context::getInstance()->pushLongKey10Sec();
        break;
    }
    case APP_EVENT_POWERON: {
        handle_poweron();
        break;
    }
    case APP_EVENT_POWEROFF: {
        handle_poweroff();
        break;
    }
    case APP_EVENT_WEIGHT_MEASURE_START: {
        handle_weight_measure_start();
        break;
    }
    case APP_EVENT_WEIGHT_MEASURE_STOP: {
        handle_weight_measure_stop();
        break;
    }
    case APP_EVENT_WIFI_CONFIG_START: {
        handle_wifi_config_start();
        break;
    }
    case APP_EVENT_WIFI_CONFIG_STOP: {
        handle_wifi_config_stop();
        break;
    }

    case APP_EVENT_TEMP_MODE_TOGGLE: {
        handle_temp_mode_toggle();
        break;
    }

    }
}

void app_event_loop_create()
{
    esp_event_loop_args_t app_loop_args = {
        .queue_size = 10,
        .task_name = "app_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 4096,
        .task_core_id = tskNO_AFFINITY
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&app_loop_args, &app_loop));
    ESP_ERROR_CHECK(esp_event_handler_register_with(app_loop, APP_EVENTS, ESP_EVENT_ANY_ID, app_loop_handler, app_loop));
}

void app_event_emit(int event)
{
    esp_event_post_to(app_loop, APP_EVENTS, event, NULL, 0, portMAX_DELAY);
}
