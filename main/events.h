#ifndef EVENTS_H
#define EVENTS_H

#include "freertos/event_groups.h"

// Bit definition(s)
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

extern EventGroupHandle_t s_event_group;

#endif  // EVENTS_H