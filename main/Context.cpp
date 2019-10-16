#include "Context.h"
#include "PoweroffState.h"

Context::Context()
{
    _state = PoweroffState::getInstance();
}

void Context::changeState(State* s)
{
    _state = s;
}

void Context::pushShortKey()
{
    _state->pushShortKey(this);
}

void Context::pushLongKey2Sec()
{
    _state->pushLongKey2Sec(this);
}

void Context::pushLongKey4Sec()
{
    _state->pushLongKey4Sec(this);
}

void Context::pushLongKey10Sec()
{
    _state->pushLongKey10Sec(this);
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
