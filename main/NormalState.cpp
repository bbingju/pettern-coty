#include "NormalState.h"
#include "PoweroffState.h"
#include "MeasureState.h"
#include "WifiConfigState.h"

void NormalState::pushShortKey(Context* c)
{
    app_event_emit(APP_EVENT_WEIGHT_MEASURE_START);
    changeState(c, MeasureState::getInstance());
}

void NormalState::pushLongKey2Sec(Context* c)
{
    changeState(c, NormalState::getInstance());
}

void NormalState::pushLongKey4Sec(Context* c)
{
    app_event_emit(APP_EVENT_POWEROFF);
    changeState(c, PoweroffState::getInstance());
}

void NormalState::pushLongKey10Sec(Context* c)
{
    app_event_emit(APP_EVENT_WIFI_CONFIG_START);
    changeState(c, WifiConfigState::getInstance());
}
