#include "Arduino.h"
#include "Context.h"
#include "app_event.h"

extern "C" void app_main()
{
    initArduino();
    Context *ctx = Context::getInstance();
    app_event_loop_create();
}
