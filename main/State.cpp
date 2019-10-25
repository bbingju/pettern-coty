#include "State.h"
#include <esp_log.h>

const static char *TAG = "State";

void State::pushShortKey(Context* c)
{
    ESP_LOGI(TAG, "short key ignored");
}

void State::pushLongKey2Sec(Context* c)
{
    ESP_LOGI(TAG, "2 sec long key ignored");
}

void State::pushLongKey4Sec(Context* c)
{
    ESP_LOGI(TAG, "4 sec long key ignored");
}

void State::pushLongKey10Sec(Context* c)
{
    ESP_LOGI(TAG, "10 sec long key ignored");
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
