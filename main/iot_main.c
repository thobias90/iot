#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_task.h"
#include "mqtt_task.h"

void app_main(void) {
  printf("Hello world!\n");
  xTaskCreate(wifi_task, "wifi_task", 4096, NULL, 5, NULL);
  xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, 5, NULL);
}
