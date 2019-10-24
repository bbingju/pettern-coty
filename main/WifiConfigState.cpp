#include "WifiConfigState.h"
#include "NormalState.h"
#include "State.h"

extern "C" {
static void wifi_setting_task(void *arg)
{
    Context *c = (Context *) arg;

    while (1) {
        c->changeButtonColor(LED::RED);
        vTaskDelay(pdMS_TO_TICKS(400));
        c->changeButtonColor(LED::BLUE);
	vTaskDelay(pdMS_TO_TICKS(400));        
    }
}
}

void WifiConfigState::pushLongKey10Sec(Context* c)
{
  c->changeState(NormalState::getInstance());
}

void WifiConfigState::begin(Context *c)
{
    c->printStringToLED("WIFI");
    c->changeButtonColor(LED::BLACK);

    xTaskCreatePinnedToCore(wifi_setting_task, "wifi_task",
                            4096, c, 3, &_task_handle, 1);
}

void WifiConfigState::end(Context *c)
{
    c->printStringToLED("    ");

    vTaskDelete(_task_handle);
}
