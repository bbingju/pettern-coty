#ifndef LED_H
#define LED_H

#include <stdint.h>
#include <esp_timer.h>

class LED {

public:
    LED();
    ~LED();

    enum COLOR {
        BLACK = 0,
        RED   = 2,
        GREEN = 4,
        BLUE  = 8,
    };

    void changeButtonColor(COLOR);
    void printString(const char *str);

    int _clock_pin;
    int _data_pin;
    int _latch_pin;
    int _enable_pin;
    int _clr_pin;

    uint8_t _data[3];
    char _string[5];
    COLOR _button_color;

    esp_timer_handle_t _timer;
    esp_timer_create_args_t _timer_args;

private:
    void start();
    void stop();
    void set(const char *str);
};

#endif /* LED_H */
