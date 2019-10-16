#include "LED.h"
#include <string.h>
#include <esp_log.h>
#include <driver/gpio.h>

const uint32_t segment_ascii[96] = {
    0b11111111111111111,	/* (space)  */
    0b01111111111110011,	/* ! */
    0b11111110111111011,	/* " */
    0b10101010111000011,	/* # */
    0b10101010101000100,	/* $ */
    0b10001000101100110,	/* % */
    0b10110110010001110,	/* & */
    0b11111110111111111,	/* ' */
    0b11110101111111111,	/* ( */
    0b11011111011111111,	/* ) */
    0b10000000011111111,	/* * */
    0b10101010111111111,	/* + */
    0b11011111111111111,	/* , */
    0b10111011111111111,	/* - */
    0b01111111111111111,	/* . */
    0b11011101111111111,	/* / */
    0b11011101100000000,	/* 0 */
    0b11111101111110011,	/* 1 */
    0b10111011110001000,	/* 2 */
    0b11111011111000000,	/* 3 */
    0b10111011101110011,	/* 4 */
    0b10110111101001100,	/* 5 */
    0b10111011100000100,	/* 6 */
    0b11111111111110000,	/* 7 */
    0b10111011100000000,	/* 8 */
    0b10111011101000000,	/* 9 */
    0b11101110111111111,	/* : */
    0b11011110111111111,	/* ; */
    0b10110101111111111,	/* < */
    0b10111011111001111,	/* = */
    0b11011011011111111,	/* > */
    0b01101011111111000,	/* ? */
    0b11111010100001000,	/* @ */
    0b10111011100110000,	/* A */
    0b11101010111000000,	/* B */
    0b11111111100001100,	/* C */
    0b11101110111000000,	/* D */
    0b10111111100001100,	/* E */
    0b10111111100111100,	/* F */
    0b11111011100000100,	/* G */
    0b10111011100110011,	/* H */
    0b11101110111001100,	/* I */
    0b11111111110000011,	/* J */
    0b10110101100111111,	/* K */
    0b11111111100001111,	/* L */
    0b11111101000110011,	/* M */
    0b11110111000110011,	/* N */
    0b11111111100000000,	/* O */
    0b10111011100111000,	/* P */
    0b11110111100000000,	/* Q */
    0b10110011100111000,	/* R */
    0b10111011101000100,	/* S */
    0b11101110111111100,	/* T */
    0b11111111100000011,	/* U */
    0b11011101100111111,	/* V */
    0b11010111100110011,	/* W */
    0b11010101011111111,	/* X */
    0b10111011101000011,	/* Y */
    0b11011101111001100,	/* Z */
    0b11101110111101101,	/* [ */
    0b11110111011111111,	/* \ */
    0b11101110111011110,	/* ] */
    0b11010111111111111,	/* ^ */
    0b11111111111001111,	/* _ */
    0b11111111011111111,	/* ` */
    0b10101111110001111,	/* a */
    0b10101111100011111,	/* b */
    0b10111111110011111,	/* c */
    0b11101011111100011,	/* d */
    0b10011111110011111,	/* e */
    0b10101010111111101,	/* f */
    0b10101110101011110,	/* g */
    0b10101111100111111,	/* h */
    0b11101111111111111,	/* i */
    0b11101110110011111,	/* j */
    0b11100100111111111,	/* k */
    0b11111111100111111,	/* l */
    0b10101011110110111,	/* m */
    0b10101111110111111,	/* n */
    0b10101111110011111,	/* o */
    0b10111110100111110,	/* p */
    0b10101110101111110,	/* q */
    0b10111111110111111,	/* r */
    0b10101111101011110,	/* s */
    0b10111111100011111,	/* t */
    0b11101111110011111,	/* u */
    0b11011111110111111,	/* v */
    0b11010111110110111,	/* w */
    0b11010101011111111,	/* x */
    0b11111010111100011,	/* y */
    0b10011111111011111,	/* z */
    0b10101110111101101,	/* { */
    0b11101110111111111,	/* | */
    0b11101010111011110,	/* } */
    0b10011001111111111,	/* ~ */
    0b11111111111111111,	/* (del) */
};

extern "C" {
    void led_timer_callback(void* arg);
}

LED::LED() : _clock_pin(14), _data_pin(15), _latch_pin(13),
             _enable_pin(17), _clr_pin(16) 
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = (gpio_int_type_t) GPIO_PIN_INTR_DISABLE;
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = ((1ULL << _clock_pin) | (1ULL << _data_pin) | 
                            (1ULL << _latch_pin) | (1ULL << _enable_pin) | (1ULL << _clr_pin));
    //disable pull-down mode
    io_conf.pull_down_en = (gpio_pulldown_t) 0;
    //disable pull-up mode
    io_conf.pull_up_en = (gpio_pullup_t) 0;
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    gpio_set_level((gpio_num_t) _data_pin, 0);
    gpio_set_level((gpio_num_t) _clock_pin, 0);
    gpio_set_level((gpio_num_t) _latch_pin, 0);
    gpio_set_level((gpio_num_t) _enable_pin, 0);
    gpio_set_level((gpio_num_t) _clr_pin, 1);

    _timer_args.callback = &led_timer_callback;
    _timer_args.arg = this;
    _timer_args.name = "led_timer";

    esp_timer_create(&_timer_args, &_timer);
    start();
}

LED::~LED()
{
    esp_timer_delete(_timer);
}

extern "C" void led_update_registers(int data_pin, int clock_pin, int latch_pin, uint8_t *data)
{
    for (int j = 2; j >= 0; j--) {
        uint8_t val = data[j];
        for(int i = 0; i < 8; i++) {
            gpio_set_level((gpio_num_t) data_pin, !!(val & (1 << (7 - i))));
            gpio_set_level((gpio_num_t) clock_pin, 1);
            gpio_set_level((gpio_num_t) clock_pin, 0);
        }
    }

    gpio_set_level((gpio_num_t) latch_pin, 1);
    gpio_set_level((gpio_num_t) latch_pin, 0);
}

extern "C" void led_set_char(uint8_t *data, char c, int position)
{
    int i = c - ' ';

    if (i < 0 || i > 95)
        return;

    if (position < 0 || position > 3)
        return;

    uint32_t v = segment_ascii[i];

    data[0] = (v & 0xff);
    data[1] = (v & (0xff << 8)) >> 8;
    data[2] = (v & (0x01 << 16)) >> 16;

    data[2] |= (1 << (position + 4));
}

extern "C" void led_set_buton_color(uint8_t *data, LED::COLOR color)
{
    data[2] |= color;
}

extern "C" void led_timer_callback(void* arg)
{
    static int digit_pos = 0;

    LED *obj = (LED *) arg;

    if (digit_pos == 4)
        digit_pos = 0;

    led_set_char(obj->_data, obj->_string[digit_pos], digit_pos);
    led_set_buton_color(obj->_data, obj->_button_color);
    led_update_registers(obj->_data_pin, obj->_clock_pin,
                         obj->_latch_pin, obj->_data);

    digit_pos++;
}

void LED::changeButtonColor(COLOR color)
{
    _button_color = color;
}

void LED::printString(const char* str)
{
    stop();
    set(str);
    start();
}

void LED::start()
{
    esp_timer_start_periodic(_timer, 300);
}

void LED::stop()
{
    esp_timer_stop(_timer);
}

void LED::set(const char *str)
{
    strncpy(_string, str, 4);
}
