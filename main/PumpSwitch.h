#ifndef PUMPSWITCH_H
#define PUMPSWITCH_H

#include "Switch.h"

class PumpSwitch: public Switch {

public:
    PumpSwitch() : Switch(4) {}
};

#endif /* PUMPSWITCH_H */
