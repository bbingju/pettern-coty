#ifndef OFFSTATE_H
#define OFFSTATE_H

#include "Context.h"

class OffState: public State {

public:
    static OffState* getInstance() {
	static OffState instance;
	return &instance;
    }

    void buttonPressedShort(Context* c);

private:
    OffState() {}

    // OffState(OffState const&);
    // void operator=(OffState const&);
  esp_timer_handle_t _timer;
  esp_timer_create_args_t _timer_args;

  void begin(Context *c);
  void end(Context *c);
};

#endif /* OFFSTATE_H */
