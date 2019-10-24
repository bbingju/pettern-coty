#ifndef NORMALSTATE_H
#define NORMALSTATE_H

#include "Context.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class NormalState : public State {

public:
  static NormalState *getInstance() {
    static NormalState instance;
    return &instance;
  }

  void pushShortKey(Context *c);
  void pushLongKey2Sec(Context *c);
  void pushLongKey4Sec(Context *c);
  void pushLongKey10Sec(Context *c);

private:
  NormalState() : _temp_mode(Context::getInstance()->getTempMode()) {}

  TempMode &_temp_mode;
  TaskHandle_t _task_handle;
  void begin(Context *);
  void end(Context *);
};

#endif /* NORMALSTATE_H */
