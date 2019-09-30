#include "TEC.h"
#include <esp_log.h>

const static char* TAG = "TEC";

void TEC::heatUp()
{
    _state = HEATUP;
    updateState();
}

void TEC::coolDown()
{
    _state = COOLDOWN;
    updateState();
}

void TEC::stop()
{
    _state = STOP;
    updateState();
}

void TEC::updateState()
{
    ESP_LOGI(TAG, "%s", stateToString());

    if (_state == HEATUP) {
        direction_a.off();
        direction_b.on();
    } else if (_state == COOLDOWN) {
        direction_a.on();
        direction_b.off();
    } else {
        direction_a.off();
        direction_b.off();
    }
}

bool TEC::isHeating()
{
    return _state == HEATUP;
}

bool TEC::isCooling()
{
    return _state == COOLDOWN;
}

bool TEC::isStopped()
{
    return _state == STOP;
}

const char* TEC::stateToString()
{
    switch (_state) {
    case HEATUP:
        return "Heating";
    case COOLDOWN:
        return "Cooling";
    default:
        return "Stopped";
    }
}
