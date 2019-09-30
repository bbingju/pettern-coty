#ifndef CONTEXT_H
#define CONTEXT_H

#include "TemperatureSensor.h"
#include "PumpSwitch.h"
#include "FanSwitch.h"
#include "TEC.h"
#include "LED.h"
#include "Button.h"
#include "TempMode.h"

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

    TempMode& getTempMode();
    void toggleTempMode();
    bool isColdMode();
    float getTargetTemperature();

    void pumpOn();
    void pumpOff();

    void fanOn();
    void fanOff();

    void tecHeatUp();
    void tecCoolDown();
    void tecStop();
    bool isTecHeating();
    bool isTecCooling();
    bool isTecStopped();
    const char *tecState();

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
    TempMode _temp_mode;
};


#endif /* CONTEXT_H */
