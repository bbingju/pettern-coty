#ifndef TEC_H
#define TEC_H

#include "Switch.h"

class TEC {

public:
    enum class TEC_STATE {
        STOP,
        HEATUP,
        COOLDOWN
    };

    TEC() : pwm(18), direction_a(19), direction_b(21) {
        _state = TEC_STATE::STOP;
        pwm.on();
        updateState();
    }

    void heapUp();
    void coolDown();
    void stop();
    bool isHeating();
    bool isCooling();
    bool isStopped();

private:
    Switch pwm;
    Switch direction_a;
    Switch direction_b;

    TEC_STATE _state;

    void updateState();
};

#endif /* TEC_H */
