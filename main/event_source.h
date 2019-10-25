#ifndef EVENT_SOURCE_H_
#define EVENT_SOURCE_H_

#include "esp_event.h"
#include "esp_event_loop.h"

#ifdef __cplusplus
extern "C" {
#endif

// Declarations for the event source
struct app_event_arg {
    int data;
};

ESP_EVENT_DECLARE_BASE(APP_EVENTS);         // declaration of the task events family

enum {
    APP_EVENT_BTN_JUST_DOWN,    // Occured when the button just down

    APP_EVENT_BTN_SHORT,        // Occured when the button up
    APP_EVENT_BTN_LONG_2SEC,    // Occured when the button up
    APP_EVENT_BTN_LONG_6SEC,    // Occured when the button up
    APP_EVENT_BTN_LONG_10SEC,   // Occured when the button up

    APP_EVENT_BTN_PRESSING_2_4,
    APP_EVENT_BTN_PRESSING_6_8,
    APP_EVENT_BTN_PRESSING_10_12,

    APP_EVENT_TEMP_MODE_TOGGLE,
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_
