#ifndef STATE_H
#define STATE_H

#include "Context.h"

class Context;

class State {

public:

    State() {}
    ~State() {}

    virtual void changeState(Context* c, State* s);
    virtual void pushShortKey(Context* c);
    virtual void pushLongKey2Sec(Context* c);
    virtual void pushLongKey4Sec(Context* c);
    virtual void pushLongKey10Sec(Context* c);

    // void operator=(State const&);

private:
    // State() {}
    // State(State const&);

};

#endif /* STATE_H */
