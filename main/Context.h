#ifndef CONTEXT_H
#define CONTEXT_H

#include "TemperatureSensor.h"
#include "PumpSwitch.h"
#include "FanSwitch.h"
#include "TEC.h"
#include "LED.h"
#include "Button.h"

#include "State.h"
#include "app_event.h"

class State;

class Context {
public:
    static Context* getInstance() {
        static Context instance;
        return &instance;
    }

    void pushShortKey();
    void pushLongKey2Sec();
    void pushLongKey4Sec();
    void pushLongKey10Sec();

    void changeState(State* s);

    float readCurrentTemperature();
    void printStringToLED(const char *);

private:
    Context();
    // Context(Context const&);
    // void operator=(Context const&);

    friend class State;
    State* _state;

    TemperatureSensor _temp_sensor;
    PumpSwitch _pump;
    FanSwitch _fan;
    TEC _tec;
    LED _led;
    Button _button;
};


#endif /* CONTEXT_H */
