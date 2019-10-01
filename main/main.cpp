#include "Arduino.h"
#include "Context.h"
#include "app_event.h"

//const char *TAG = "Main";

extern "C" void app_main()
{
    initArduino();
    Context *ctx = Context::getInstance();
    app_event_loop_create();
}
