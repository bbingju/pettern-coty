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
    APP_EVENT_BTN_SHORT,        //
    APP_EVENT_BTN_LONG_2SEC,
    APP_EVENT_BTN_LONG_4SEC,    //
    APP_EVENT_BTN_LONG_10SEC,   //

    APP_EVENT_TEMP_MODE_TOGGLE,
};

#ifdef __cplusplus
}
#endif

#endif // #ifndef EVENT_SOURCE_H_
