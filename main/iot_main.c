#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_task.h"
#include "events.h"
#include "wifi_task.h"
#include "gpio_task.h"

EventGroupHandle_t s_event_group = NULL;

void app_main(void) {
  s_event_group = xEventGroupCreate();

  xTaskCreate(wifi_task, "wifi_task", 4096, NULL, 5, NULL);
  xTaskCreate(mqtt_task, "mqtt_task", 8192, NULL, 5, NULL);
  xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 5, NULL);
}
