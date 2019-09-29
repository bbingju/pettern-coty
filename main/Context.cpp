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
    return _temp_sensor.read();
}

void Context::printStringToLED(const char *str)
{
    _led.printString(str);
}
