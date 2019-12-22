#ifndef MEASURESTATE_H
#define MEASURESTATE_H

#include "Context.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


class MeasureState : public State {

public:
  static MeasureState *getInstance() {
    static MeasureState instance;
    return &instance;
  }

  void buttonPressedShort(Context *c);

private:
  MeasureState() {}

  TaskHandle_t _task_handle;
  void begin(Context *c);
  void end(Context *c);
};

#endif /* MEASURESTATE_H */
