#include "PoweroffState.h"
#include "NormalState.h"

void PoweroffState::pushShortKey(Context* c)
{
    app_event_emit(APP_EVENT_POWERON);
    changeState(c, NormalState::getInstance());
}
