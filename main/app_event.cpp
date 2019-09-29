#include "app_event.h"
#include <esp_log.h>
#include "Context.h"

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
        default:
            event = "Unkown event";
        }
    }
    return event;
}

static void handle_poweron()
{
    Context *c = Context::getInstance();
    c->printStringToLED("COLD");
}

static void handle_poweroff()
{
    Context *c = Context::getInstance();
    c->printStringToLED("    ");
}

static void handle_weight_measure_start()
{
    Context *c = Context::getInstance();
    c->printStringToLED("MSRT");
}

static void handle_weight_measure_stop()
{
    Context *c = Context::getInstance();
    c->printStringToLED("MSTP");
}

static void handle_wifi_config_start()
{
    Context *c = Context::getInstance();
    c->printStringToLED("WSRT");
}

static void handle_wifi_config_stop()
{
    Context *c = Context::getInstance();
    c->printStringToLED("WSTP");
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

    }
}

void app_event_loop_create()
{
    esp_event_loop_args_t app_loop_args = {
        .queue_size = 5,
        .task_name = "app_loop_task",
        .task_priority = uxTaskPriorityGet(NULL),
        .task_stack_size = 2048,
        .task_core_id = tskNO_AFFINITY
    };

    ESP_ERROR_CHECK(esp_event_loop_create(&app_loop_args, &app_loop));
    ESP_ERROR_CHECK(esp_event_handler_register_with(app_loop, APP_EVENTS, ESP_EVENT_ANY_ID, app_loop_handler, app_loop));
}

void app_event_emit(int event)
{
    esp_event_post_to(app_loop, APP_EVENTS, event, NULL, 0, portMAX_DELAY);
}
