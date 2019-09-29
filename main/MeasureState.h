#ifndef MEASURESTATE_H
#define MEASURESTATE_H

#include "Context.h"

class MeasureState: public State {

public:
    static MeasureState* getInstance() {
        static MeasureState instance;
        return &instance;
    }

    virtual void pushShortKey(Context* c);

private:
    MeasureState() {}
};

#endif /* MEASURESTATE_H */
