#ifndef CONTEXT_H
#define CONTEXT_H

#include "TemperatureSensor.h"
#include "PumpSwitch.h"
#include "FanSwitch.h"
#include "TEC.h"
#include "LED.h"
#include "Button.h"
#include "TempMode.h"
#include "WifiInfo.h"

#include "State.h"
#include "app_event.h"

class State;

class Context
{
public:
    static Context *getInstance()
    {
        static Context instance;
        return &instance;
    }

    void buttonPressedShort();
    void buttonPressedLong();
    void buttonPressedDouble();
    void buttonPressing(Button *);
    void buttonPressingPerSec();
    void buttonPressing2Sec();
    void buttonPressing6Sec();
    void buttonPressing10Sec();
    int  buttonPressingDuration();
    bool isButtonPressing();
    bool isButtonValid();

    void changeState(State *s);
    void stackState(State *s);
    void unstackState();

    State *getState();
    bool isNormalState();

    String getDeviceName();

    float readCurrentTemperature();

    void changeButtonColor(LED::COLOR);
    void printStringToLED(const char *);
    void printWeightToLED(const float value);
    void printTemperatureToLED(const float);

    TempMode &getTempMode();
    void toggleTempMode();
    bool isColdMode();
    float getTargetTemperature();
    bool setTargetTemperature(TempMode::TEMP_MODE mode, float target);

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

    void setupWiFi();
    void saveWiFiInfo(String ssid, String password);
    String getWiFiSSID();
    String getWiFiPassword();

    float getLastWeight();
    void setLastWeight(float weight);


private:
    Context();
    // Context(Context const&);
    // void operator=(Context const&);

    String _device_name;
    const int state_stack_max{3};
    friend class State;
    State *_state_stack[3] {0};
    int _state_stack_idx{0};

    TemperatureSensor _temp_sensor;
    PumpSwitch _pump;
    FanSwitch _fan;
    TEC _tec;
    LED _led;
    Button _button;
    TempMode _temp_mode;
    WifiInfo _wifi_info;
    float _last_weight;
};

#endif /* CONTEXT_H */
