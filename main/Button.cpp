#include "Button.h"
#include <esp_log.h>
// #include "esp_sleep.h"
#include <driver/gpio.h>
#include "app_event.h"

//static const char* TAG = "button";

#define SEC(n) (1000 * 1000 * (n))

static void button_timer_callback(void* arg)
{
    Button *btn = (Button *) arg;

    static int64_t pushed_tick = 0;
    static int prev_state = 1;
    static int current_state = 1;
    static bool event_emitted = false;

    prev_state = current_state;
    current_state = btn->read();

    if (prev_state == 1 && current_state == 0) { // 버튼 눌림
        pushed_tick = esp_timer_get_time();
        event_emitted = false;
    } else if (prev_state == 0 && current_state == 0) { // 눌림 상태 지속
        int64_t duration = esp_timer_get_time() - pushed_tick;
        if (!event_emitted && duration >= SEC(10)) {
            event_emitted = true;
            app_event_emit(APP_EVENT_BTN_LONG_10SEC);
        }
    } else if (prev_state == 0 && current_state == 1) { // 버튼 떨어짐
        int64_t duration = esp_timer_get_time() - pushed_tick;
        if (duration >= SEC(2) && duration < SEC(4)) {
            event_emitted = true;
            app_event_emit(APP_EVENT_BTN_LONG_2SEC);
        } else if (duration >= SEC(4) && duration < SEC(10)) {
            event_emitted = true;
            app_event_emit(APP_EVENT_BTN_LONG_4SEC);
        } else {
            if (duration < SEC(1)) {
                event_emitted = true;
                app_event_emit(APP_EVENT_BTN_SHORT);
            }
        }
    } else {
        // ESP_LOGI(TAG, "other states\n");
    }
}

#if 0
extern "C" void button_task(void *arg)
{
    Button *btn = (Button *) arg;
    while (1) {
        printf("button: %u\n", btn->read());
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif //0

Button::Button(): _pin(34)
{
    _state = 1;

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_INPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << _pin);
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t) 0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t) 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    // xTaskCreatePinnedToCore(button_task, "button_task",
    //                         2048, this, 2, NULL, 1);

    _timer_args.callback = &button_timer_callback;
    _timer_args.arg = this;
    _timer_args.name = "button_timer";

    esp_timer_create(&_timer_args, &_timer);
    esp_timer_start_periodic(_timer, 100 * 1000);
}

int Button::read()
{
    _state = gpio_get_level((gpio_num_t) _pin);
    return _state;
}
