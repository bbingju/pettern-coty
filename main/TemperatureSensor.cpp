#include <math.h>
#include <driver/adc.h>
#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

float TemperatureSensor::read()
{
    static float Rt,Adc,T,LnRt;
    Adc = (float)adc1_get_raw(ADC1_CHANNEL_0);
    Rt = (10000.0 * Adc)/(4096.0 - Adc);
    LnRt = log(Rt);
    T = (1.0/(Ra + Rb*LnRt + Rc*pow(LnRt, 3.0)))-273.15;
    return T;
}

float TemperatureSensor::readAV()
{
    float val = 0.0f;
    int i = 0;
    for (i = 0; i < 20; i++) {
        val += read();
    }

    return (val / i);
}
