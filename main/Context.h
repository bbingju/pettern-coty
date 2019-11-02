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

#include <Preferences.h>

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
    void buttonPressing2Sec();
    void buttonPressing6Sec();
    void buttonPressing10Sec();
    bool isButtonPressing();
    bool isButtonValid();

    void changeState(State* s);
    void stackState(State *s);
    void unstackState();

    State *getState();
    bool isNormalState();

    float readCurrentTemperature();

    void changeButtonColor(LED::COLOR);
    void printStringToLED(const char *);
    void printTemperatureToLED(const float);

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

    const int state_stack_max{3};
    friend class State;
    State *_state_stack[3]{0};
    int _state_stack_idx{0};

    Preferences _pref;
    TemperatureSensor _temp_sensor;
    PumpSwitch _pump;
    FanSwitch _fan;
    TEC _tec;
    LED _led;
    Button _button;
    TempMode _temp_mode;
};

#endif /* CONTEXT_H */
