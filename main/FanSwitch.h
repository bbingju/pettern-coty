#ifndef FANSWITCH_H
#define FANSWITCH_H

#include "Switch.h"

class FanSwitch: public Switch {

public:
    FanSwitch() : Switch(2) {}
};

#endif /* FANSWITCH_H */
