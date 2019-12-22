#include "State.h"
#include <esp_log.h>

const static char *TAG = "State";

void State::buttonPressedShort(Context* c)
{
    ESP_LOGI(TAG, "short key ignored");
}

void State::buttonPressedLong(Context *c)
{
    ESP_LOGI(TAG, "long button ignored");
}

void State::buttonPressedDouble(Context *c)
{
    ESP_LOGI(TAG, "Double button ignored");
}

void State::pressing(Context *c)
{
    ESP_LOGI(TAG, "default pressing");
}

void State::buttonPressingPerSec(Context *c)
{
    ESP_LOGI(TAG, "default pressing per Sec");
}

void State::pressing2Sec(Context *c)
{
    ESP_LOGI(TAG, "default pressing 2 Sec");
}

void State::pressing6Sec(Context *c)
{
    ESP_LOGI(TAG, "default pressing 6 Sec");
}

void State::pressing10Sec(Context *c)
{
   ESP_LOGI(TAG, "default pressing 10 sec");
}

void State::begin(Context *c)
{
    ESP_LOGI(TAG, "default begin");
}

void State::end(Context *c)
{
    ESP_LOGI(TAG, "default end");
}
