#include "MeasureState.h"
#include "NormalState.h"
#include "HX711.h"

static const char * TAG = "MEASURE_STATE";

extern "C" {
    static void loadcell_task(void *arg)
    {
        Context *c = (Context *)arg;
        HX711 scale;

        scale.begin(23, 22);

        // 영점조정
        scale.set_scale(293127.f); // this value is obtained by
                                   // calibrating the scale with known
                                   // weights; see the README for details
        scale.tare();              // reset the scale to 0

        // 영점조정 완료
        c->printStringToLED("    ");
        vTaskDelay(pdMS_TO_TICKS(300));
        c->printTemperatureToLED(0.f);
        vTaskDelay(pdMS_TO_TICKS(300));
        c->printStringToLED("    ");
        vTaskDelay(pdMS_TO_TICKS(300));
        c->printTemperatureToLED(0.f);

        while (1) {

            long int v = scale.read_average(20);
            float w;

            w = 8 * powf(10, -6) * ((float) v) + 0.5325;

            ESP_LOGI(TAG, "Loadcell - read average: %li, %.2f", v, w);

            c->printTemperatureToLED(w);

            scale.power_down(); // put the ADC in sleep mode
            vTaskDelay(pdMS_TO_TICKS(5000));
            scale.power_up();
        }
    }
}

void MeasureState::pushShortKey(Context *c)
{
    c->unstackState();
}

void MeasureState::begin(Context *c)
{
    xTaskCreatePinnedToCore(loadcell_task, "loadcell_task",
                            4096, c, 3, &_task_handle, 1);
}

void MeasureState::end(Context *c)
{
    vTaskDelete(_task_handle);  
}
