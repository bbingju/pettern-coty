#include "Button.h"
#include "app_event.h"
#include <esp_log.h>
#include <driver/gpio.h>

//static const char* TAG = "button";

#define SEC(tick) ((tick) / (1000 * 1000))

static void button_timer_callback(void* arg)
{
    Button *btn = (Button *) arg;

    static int64_t pushed_tick = 0;
    static int prev_state = 1;
    static int current_state = 1;
    static int old_duration_sec = 0;
    static int old_duration = 0;
    static int64_t last_short_tick = 0;
    static bool double_clicked = false;

    prev_state = current_state;
    current_state = btn->read();

    if (prev_state == 1 && current_state == 0) { // 버튼 눌림
        pushed_tick = esp_timer_get_time();

	if (pushed_tick - last_short_tick > 80 * 1000 && pushed_tick - last_short_tick < 300 * 1000) {
	  double_clicked = true;
	  btn->is_pressing = false;
          btn->duration_sec = 0;
          btn->duration = 0;
          btn->is_valid = false;
        } else {
          double_clicked = false;
          btn->is_pressing = true;
          btn->duration_sec = 0;
          btn->duration = 0;
          btn->is_valid = false;
        }
    } else if (prev_state == 0 && current_state == 0) { // 눌림 상태 지속

        if (!double_clicked && btn->is_pressing) {
            old_duration = btn->duration;
	    btn->duration = esp_timer_get_time() - pushed_tick;
	    old_duration_sec = btn->duration_sec;
            btn->duration_sec = SEC(btn->duration);

	    if ((old_duration != btn->duration) && (btn->duration > 300 * 1000))
	        app_event_emit(APP_EVENT_BTN_PRESSING, btn);

	    if (old_duration_sec != btn->duration_sec)
	        app_event_emit(APP_EVENT_BTN_PRESSING_PER_SEC, btn);

            if (SEC(btn->duration) >= 2 && btn->duration_sec < SEC(4)) {
                btn->is_valid = true;
                app_event_emit(APP_EVENT_BTN_PRESSING_2_4, btn);
            }
            else if (btn->duration_sec >= SEC(6) && btn->duration_sec < SEC(10)) {
                btn->is_valid = true;
                app_event_emit(APP_EVENT_BTN_PRESSING_6_8, btn);
            }
            else if (btn->duration_sec >= SEC(10) && btn->duration_sec < SEC(12)) {
                btn->is_valid = true;
                app_event_emit(APP_EVENT_BTN_PRESSING_10_12, btn);
            }
            else
                btn->is_valid = false;
        }
    } else if (prev_state == 0 && current_state == 1) { // 버튼 떨어짐
        btn->is_pressing = false;
        btn->is_valid = false;

	old_duration = btn->duration;
	btn->duration = esp_timer_get_time() - pushed_tick;
        old_duration_sec = btn->duration_sec;
	btn->duration_sec = SEC(btn->duration);

      if (!double_clicked) {
	// app_event_emit(APP_EVENT_BTN_UP, btn);

        if (btn->duration_sec == SEC(0)) {
	    last_short_tick = esp_timer_get_time();
        } else {
            app_event_emit(APP_EVENT_BTN_LONG, btn);
	    // last_short_tick = 0;
        }
      } else {
          app_event_emit(APP_EVENT_BTN_DOUBLE, btn);
          // double_clicked = false;
      }
	
    } else {
      if (!double_clicked && (last_short_tick != 0) && (esp_timer_get_time() - last_short_tick < 280 * 1000)) {
	app_event_emit(APP_EVENT_BTN_SHORT, btn);
	// last_short_tick = esp_timer_get_time();
	last_short_tick -= 150 * 1000;
      }
        // ESP_LOGI(TAG, "other states\n");
    }
}

Button::Button(): _pin(34)
{
    _state = 1;
    is_pressing = false;
    is_valid = false;

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
