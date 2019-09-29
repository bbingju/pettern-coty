#ifndef NORMALSTATE_H
#define NORMALSTATE_H

#include "Context.h"

class NormalState: public State {

public:
    static NormalState* getInstance() {
        static NormalState instance;
        return &instance;
    }

    virtual void pushShortKey(Context* c);
    virtual void pushLongKey2Sec(Context* c);
    virtual void pushLongKey4Sec(Context* c);
    virtual void pushLongKey10Sec(Context* c);

private:
    NormalState() {}

    // NormalState(NormalState const&);
    // void operator=(NormalState const&);
};


#endif /* NORMALSTATE_H */
