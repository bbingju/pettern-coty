// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
#include "Arduino.h"
#include "Context.h"

const char *TAG = "Main";

// float target_temperature_cold = 25.0f;
// float target_temperature_hot = 37.0f;
// float *target_temperature = &target_temperature_cold;
// float current_temperature;

extern "C" void app_main()
{
    initArduino();
    Context *ctx = Context::getInstance();
    app_event_loop_create();

    // Serial.begin(115200);

    // while (1) {
    //     current_temperature = ctx->readCurrentTemperature();
    //     printf("Current: %.4f C, Target %.4f\n", current_temperature, *target_temperature);
    //     // printf("button %d\n", button.read());

    //     delay(100000000);
    //     // if (current_temperature < *target_temperature) {
    //     //     if (!tec.isHeating()) {
    //     //         printf("===== Heap Up!\n");
    //     //         tec.heapUp();
    //     //         pump.on();
    //     //         fan.on();
    //     //     }
    //     // } else {
    //     //     printf("===== Stop!\n");
    //     //     tec.stop();
    //     //     pump.off();
    //     //     fan.off();
    //     // }

    //     // char temp_str[5] = { 0 };
    //     // sprintf(temp_str, "%.1f", target_temperature);
    //     // led.printString(temp_str);
    //     // delay(3000);
    // }
}
