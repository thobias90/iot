#ifndef IOT_SHARED_H
#define IOT_SHARED_H

#include "freertos/queue.h"

extern QueueHandle_t s_led_events;
extern QueueHandle_t s_led_requests;

#endif  // IOT_SHARED_H