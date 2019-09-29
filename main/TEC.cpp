#include "TEC.h"

void TEC::heapUp()
{
    _state = TEC_STATE::HEATUP;
    updateState();
}

void TEC::coolDown()
{
    _state = TEC_STATE::COOLDOWN;
    updateState();
}

void TEC::stop()
{
    _state = TEC_STATE::STOP;
    updateState();
}

void TEC::updateState()
{
    if (_state == TEC_STATE::HEATUP) {
        direction_a.off();
        direction_b.on();
    } else if (_state == TEC_STATE::COOLDOWN) {
        direction_a.on();
        direction_b.off();
    } else {
        direction_a.off();
        direction_b.off();
    }
}

bool TEC::isHeating()
{
    return _state == TEC_STATE::HEATUP;
}

bool TEC::isCooling()
{
    return _state == TEC_STATE::COOLDOWN;
}

bool TEC::isStopped()
{
    return _state == TEC_STATE::STOP;
}

