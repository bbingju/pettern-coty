#include "WifiConfigState.h"
#include "NormalState.h"

void WifiConfigState::pushLongKey10Sec(Context* c)
{
    app_event_emit(APP_EVENT_WIFI_CONFIG_STOP);
    changeState(c, NormalState::getInstance());
}
