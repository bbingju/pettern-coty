#ifndef WIFICONFIGSTATE_H
#define WIFICONFIGSTATE_H

#include "Context.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class WifiConfigState: public State {

public:
    static WifiConfigState* getInstance() {
        static WifiConfigState instance;
        return &instance;
    }

    void pushLongKey10Sec(Context* c);
    void pressing10Sec(Context *c);

private:
  WifiConfigState() {}

  TaskHandle_t _task_handle;

  void begin(Context *);
  void end(Context *);
};

#endif /* WIFICONFIGSTATE_H */
