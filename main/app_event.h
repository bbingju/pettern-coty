#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "event_source.h"

#ifdef __cplusplus
extern "C" {
#endif

    void app_event_loop_create();
    void app_event_emit(int event);

#ifdef __cplusplus
}
#endif

#endif /* APP_EVENT_H */
