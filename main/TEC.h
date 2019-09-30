#ifndef TEC_H
#define TEC_H

#include "Switch.h"

class TEC {

public:
    enum TEC_STATE {
        STOP,
        HEATUP,
        COOLDOWN
    };

    TEC() : pwm(18), direction_a(19), direction_b(21), _state(STOP) {
        pwm.on();
        updateState();
    }

    void heatUp();
    void coolDown();
    void stop();
    bool isHeating();
    bool isCooling();
    bool isStopped();
    const char* stateToString();

private:
    Switch pwm;
    Switch direction_a;
    Switch direction_b;

    TEC_STATE _state;

    void updateState();
};

#endif /* TEC_H */
