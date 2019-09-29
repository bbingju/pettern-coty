#ifndef POWEROFFSTATE_H
#define POWEROFFSTATE_H

#include "Context.h"

class PoweroffState: public State {

public:
    static PoweroffState* getInstance() {
        static PoweroffState instance;
        return &instance;
    }

    virtual void pushShortKey(Context* c);

private:
    PoweroffState() {}

    // PoweroffState(PoweroffState const&);
    // void operator=(PoweroffState const&);
};

#endif /* POWEROFFSTATE_H */
