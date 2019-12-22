#ifndef BUTTON_H
#define BUTTON_H

#include <esp_timer.h>

class Button {

public:
    Button();

    int read();

    bool is_pressing;
    int duration_sec;
    int duration;
    bool is_valid;

private:
    int _pin;
    int _state;

    esp_timer_handle_t _timer;
    esp_timer_create_args_t _timer_args;
};

#endif /* BUTTON_H */
