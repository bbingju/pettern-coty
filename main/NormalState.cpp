#include "NormalState.h"
#include "OffState.h"
#include "MeasureState.h"
#include "WifiConfigState.h"
#include <esp_log.h>
#include "PID.h"

static const char *TAG = "NORMAL_STATE";

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

extern "C" {
    static void pid_task(void *arg)
    {
      Context *c = (Context *)arg;
      static char target_str[5] = {0};

      while (1) {

        float current = c->readCurrentTemperature();
        float target = c->getTargetTemperature();
        float target_l = target - 0.5f;
        float target_h = target + 0.5f;

        if (c->isColdMode()) {
          if (current > target_h) {
            if (!c->isTecCooling()) {
              c->tecCoolDown();
              c->pumpOn();
              c->fanOn();
            }
          } else if (current < target_l) {
            if (!c->isTecHeating()) {
              c->tecHeatUp();
              c->pumpOn();
              c->fanOn();
            }
          } else {
            if (!c->isTecStopped()) {
              c->tecStop();
              c->pumpOff();
              c->fanOff();
            }
          }
          ESP_LOGI(TAG, "COLD MODE - Current: %.2f, Target: %.2f => %s",
                   current, target, c->tecState());
        } else {
          if (current < target_l) {
            if (!c->isTecHeating()) {
              c->tecHeatUp();
              c->pumpOn();
              c->fanOn();
            }
          } else if (current > target_h) {
            if (!c->isTecCooling()) {
              c->tecCoolDown();
              c->pumpOn();
              c->fanOn();
            }
          } else {
            if (!c->isTecStopped()) {
              c->tecStop();
              c->pumpOff();
              c->fanOff();
            }
          }
          ESP_LOGI(TAG, "HOT MODE - Current: %.2f, Target: %.2f => %s", current,
                   target, c->tecState());
        }

        if (c->isNormalState() && !c->isButtonValid()) {
          snprintf(target_str, 4, "%.1f", target);
          c->printStringToLED(target_str);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
      }
    }
}

void NormalState::pushShortKey(Context* c)
{
    c->stackState(MeasureState::getInstance());
}

void NormalState::pushLongKey2Sec(Context* c)
{
    app_event_emit(APP_EVENT_TEMP_MODE_TOGGLE);
}

void NormalState::pushLongKey4Sec(Context* c)
{
    c->changeState(OffState::getInstance());
}

void NormalState::pushLongKey10Sec(Context* c)
{
    c->changeState(WifiConfigState::getInstance());
}

void NormalState::pressing2Sec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    if (count == 0) {
        c->printStringToLED(flag ? (c->isColdMode() ? "HOT " : "COLD") : "    ");
        flag = !flag;
    }

    ++count;
    count %= 3;
}

void NormalState::pressing6Sec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    if (count == 0) {
        c->printStringToLED(flag ? " CU " : "    ");
        flag = !flag;
    }

    ++count;
    count %= 3;
}

void NormalState::pressing10Sec(Context *c)
{
    static bool flag = true;
    static int count = 0;

    if (count == 0) {
        c->printStringToLED(flag ? "WIFI" : "    ");
        flag = !flag;
    }

    ++count;
    count %= 3;
}

void NormalState::begin(Context *c)
{
    xTaskCreatePinnedToCore(pid_task, "pid_task",
			    4096, c, 3, &_task_handle, 1);
}

void NormalState::end(Context *c)
{

    c->tecStop();
    c->pumpOff();
    c->fanOff();

    vTaskDelete(_task_handle);
}
