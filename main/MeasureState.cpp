#include "MeasureState.h"
#include "NormalState.h"

void MeasureState::pushShortKey(Context* c)
{
    app_event_emit(APP_EVENT_WEIGHT_MEASURE_STOP);
    changeState(c, NormalState::getInstance());
}
