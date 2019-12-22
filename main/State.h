#ifndef STATE_H
#define STATE_H

#include "Context.h"

class Context;

class State {

public:
  State() {}
  ~State() {}

  virtual void buttonPressedShort(Context *c);
  virtual void buttonPressedLong(Context *c);
  virtual void buttonPressedDouble(Context *c);
  virtual void pressing(Context *c);
  virtual void buttonPressingPerSec(Context *c);
  virtual void pressing2Sec(Context *c);
  virtual void pressing6Sec(Context *c);
  virtual void pressing10Sec(Context *c);

  virtual void begin(Context *c);
  virtual void end(Context *);
  // void operator=(State const&);

private:
  // State() {}
  // State(State const&);
};

#endif /* STATE_H */
