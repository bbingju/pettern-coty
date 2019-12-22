#include "WifiConfigState.h"
#include "NormalState.h"
#include "State.h"

extern "C" {
static void wifi_setting_task(void *arg)
{
    Context *c = (Context *) arg;

    while (1) {
        c->printStringToLED("WIFI");

        c->changeButtonColor(LED::RED);
        vTaskDelay(pdMS_TO_TICKS(400));
        c->changeButtonColor(LED::BLUE);
	vTaskDelay(pdMS_TO_TICKS(400));        
    }
}
}

void WifiConfigState::buttonPressedLong(Context *c)
{
    int duration = c->buttonPressingDuration();

    // ESP_LOGI(TAG, "duration: %d\n", duration);

    if (duration >= 4 && duration < 8) {
	c->changeState(NormalState::getInstance());
    }
}

void WifiConfigState::pressing10Sec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    if (count == 0) {
        c->printStringToLED(flag ? (c->isColdMode() ? "COLD" : "HOT") : "    ");
        flag = !flag;
    }

    ++count;
    count %= 3;
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
    c->changeButtonColor(LED::BLACK);

    vTaskDelete(_task_handle);
}
