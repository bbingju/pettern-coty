#include "OffState.h"
#include "NormalState.h"

extern "C" {
static void timer_callback(void* arg)
{
    Context *c = (Context *) arg;
    c->changeButtonColor(LED::BLACK);
    c->printStringToLED("    ");
}
}

void OffState::pushShortKey(Context *c) {
  // Power-on
  c->changeState(NormalState::getInstance());
}

void OffState::begin(Context *c)
{
    c->printStringToLED(" CU ");

    _timer_args.callback = &timer_callback;
    _timer_args.arg = c;
    _timer_args.name = "onpower_timer";
    esp_timer_create(&_timer_args, &_timer);
    esp_timer_start_once(_timer, 1000 * 1000 * 2);
}

void OffState::end(Context *c)
{
  c->printStringToLED(" ON ");
  vTaskDelay(pdMS_TO_TICKS(1200));

  if (c->isColdMode()) {
    c->changeButtonColor(LED::BLUE);
    c->printStringToLED("COLD");
  } else {
    c->changeButtonColor(LED::RED);
    c->printStringToLED("HOT ");
  }
  vTaskDelay(pdMS_TO_TICKS(500));
}
