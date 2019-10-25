#include "Context.h"
#include "NormalState.h"
#include "OffState.h"

Context::Context()
{
    _state_stack[_state_stack_idx] = OffState::getInstance();

    // get preferences
    _pref.begin("cotypref");
    float hot_mode_temp = _pref.getFloat("hot_mode_temp", 37.f);
    float cold_mode_temp = _pref.getFloat("cold_mode_temp", 25.f);
    // Serial.printf("hot_mode_temp = %.1f\n", hot_mode_temp);
    // Serial.printf("cold_mode_temp = %.1f\n", cold_mode_temp);
    _temp_mode.setTargetTemp(TempMode::COLD, cold_mode_temp);
    _temp_mode.setTargetTemp(TempMode::HOT, hot_mode_temp);
    _pref.end();
}

void Context::changeState(State *s)
{
    while (_state_stack_idx > 0) {
        unstackState();
    }

    State *current = _state_stack[_state_stack_idx];

    current->end(this);
    _state_stack[_state_stack_idx] = current = s;
    current->begin(this);
}

void Context::stackState(State *s)
{
    if (_state_stack_idx <= state_stack_max - 1) {
        _state_stack[++_state_stack_idx] = s;
        _state_stack[_state_stack_idx]->begin(this);
    }
}

void Context::unstackState()
{
    if (_state_stack_idx > 0) {
        _state_stack[_state_stack_idx]->end(this);
        // _state_stack[_state_stack_idx] = NULL;
        _state_stack_idx--;
    }
}

State * Context::getState()
{
    return _state_stack[_state_stack_idx];
}

bool Context::isNormalState()
{
    return _state_stack[_state_stack_idx] == NormalState::getInstance();
}

bool Context::isButtonPressing()
{
    return _button.is_pressing;
}

bool Context::isButtonValid()
{
    return _button.is_valid;
}

void Context::pushShortKey()
{
    _state_stack[_state_stack_idx]->pushShortKey(this);
}

void Context::pushLongKey2Sec()
{
    _state_stack[_state_stack_idx]->pushLongKey2Sec(this);
}

void Context::pushLongKey4Sec()
{
    _state_stack[_state_stack_idx]->pushLongKey4Sec(this);
}

void Context::pushLongKey10Sec()
{
    _state_stack[_state_stack_idx]->pushLongKey10Sec(this);
}

void Context::buttonPressing2Sec()
{
    _state_stack[_state_stack_idx]->pressing2Sec(this);
}

void Context::buttonPressing6Sec()
{
    _state_stack[_state_stack_idx]->pressing6Sec(this);
}

void Context::buttonPressing10Sec()
{
    _state_stack[_state_stack_idx]->pressing10Sec(this);
}

float Context::readCurrentTemperature()
{
    return _temp_sensor.readAV();
}

void Context::changeButtonColor(LED::COLOR color)
{
    _led.changeButtonColor(color);
}

void Context::printStringToLED(const char *str)
{
    _led.printString(str);
}

TempMode& Context::getTempMode()
{
    return _temp_mode;
}

void Context::toggleTempMode()
{
    _temp_mode.toggle();
}

bool Context::isColdMode()
{
    return !_temp_mode.isHotMode();
}

float Context::getTargetTemperature()
{
    return _temp_mode.getTargetTemp();
}

void Context::pumpOn()
{
    _pump.on();
}

void Context::pumpOff()
{
    _pump.off();
}

void Context::fanOn()
{
    _fan.on();
}

void Context::fanOff()
{
    _fan.off();
}

void Context::tecHeatUp()
{
    _tec.heatUp();
}

void Context::tecCoolDown()
{
    _tec.coolDown();
}

void Context::tecStop()
{
    _tec.stop();
}

bool Context::isTecHeating()
{
    return _tec.isHeating();
}

bool Context::isTecCooling()
{
    return _tec.isCooling();
}

bool Context::isTecStopped()
{
    return _tec.isStopped();
}

const char * Context::tecState()
{
    return _tec.stateToString();
}
