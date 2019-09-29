#include <driver/gpio.h>
#include "Switch.h"

Switch::Switch(int pin)
{
    _pin = pin;
    _state = 0;

    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1ULL << pin);
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t) 0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t) 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    update();
}

void Switch::update()
{
    gpio_set_level((gpio_num_t) _pin, _state);
}

void Switch::on()
{
    _state = 1;
    update();
}

void Switch::off()
{
    _state = 0;
    update();
}

int Switch::state()
{
    return _state;
}
