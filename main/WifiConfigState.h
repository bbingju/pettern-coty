#ifndef WIFICONFIGSTATE_H
#define WIFICONFIGSTATE_H

#include "Context.h"

class WifiConfigState: public State {

public:
    static WifiConfigState* getInstance() {
        static WifiConfigState instance;
        return &instance;
    }

    virtual void pushLongKey10Sec(Context* c);

private:
    WifiConfigState() {}
};

#endif /* WIFICONFIGSTATE_H */
