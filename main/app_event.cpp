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
        case APP_EVENT_BTN_JUST_DOWN:
            event = "APP_EVENT_BTN_JUST_DOWN";
            break;
        case APP_EVENT_BTN_UP:
            event = "APP_EVENT_BTN_UP";
            break;
        case APP_EVENT_BTN_SHORT:
            event = "APP_EVENT_BTN_SHORT";
            break;
        case APP_EVENT_BTN_LONG:
            event = "APP_EVENT_BTN_LONG";
            break;
        case APP_EVENT_BTN_DOUBLE:
            event = "APP_EVENT_BTN_DOUBLE";
            break;
        case APP_EVENT_BTN_LONG_2SEC:
            event = "APP_EVENT_BTN_LONG_2SEC";
            break;
        case APP_EVENT_BTN_LONG_6SEC:
            event = "APP_EVENT_BTN_LONG_6SEC";
            break;
        case APP_EVENT_BTN_LONG_10SEC:
            event = "APP_EVENT_BTN_LONG_10SEC";
            break;
        case APP_EVENT_BTN_PRESSING:
            event = "APP_EVENT_BTN_PRESSING";
            break;
        case APP_EVENT_BTN_PRESSING_PER_SEC:
            event = "APP_EVENT_BTN_PRESSING_PER_SEC";
            break;
        case APP_EVENT_BTN_PRESSING_2_4:
            event = "APP_EVENT_BTN_PRESSING_2_4";
            break;
        case APP_EVENT_BTN_PRESSING_6_8:
            event = "APP_EVENT_BTN_PRESSING_6_8";
            break;
        case APP_EVENT_BTN_PRESSING_10_12:
            event = "APP_EVENT_BTN_PRESSING_10_12";
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

    case APP_EVENT_BTN_JUST_DOWN: {
        // Context::getInstance()->buttonJustPressed();
        break;
    }
    case APP_EVENT_BTN_UP: {
        break;
    }
    case APP_EVENT_BTN_SHORT: {
        // struct app_event_arg *d = (struct app_event_arg *) event_data;
        Context::getInstance()->buttonPressedShort();
        break;
    }
    case APP_EVENT_BTN_LONG: {
        Context::getInstance()->buttonPressedLong();
        break;
    }
    case APP_EVENT_BTN_DOUBLE: {
        Context::getInstance()->buttonPressedDouble();
        break;
    }
    case APP_EVENT_BTN_PRESSING: {
	Context::getInstance()->buttonPressing((Button *) event_data);
        break;
    }
    case APP_EVENT_BTN_PRESSING_PER_SEC: {
        Context::getInstance()->buttonPressingPerSec();
        break;
    }
    case APP_EVENT_BTN_PRESSING_2_4: {
        Context::getInstance()->buttonPressing2Sec();
        break;
    }
    case APP_EVENT_BTN_PRESSING_6_8: {
        Context::getInstance()->buttonPressing6Sec();
        break;
    }
    case APP_EVENT_BTN_PRESSING_10_12: {
        Context::getInstance()->buttonPressing10Sec();
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

void app_event_emit(int event, void *param)
{
    esp_event_post_to(app_loop, APP_EVENTS, event, param, 0, portMAX_DELAY);
}
