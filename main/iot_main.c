#include <stdio.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "wifi_connect.h"

void app_main(void) {
  printf("Hello world!\n");
  nvs_flash_init();
  esp_netif_init();
  esp_event_loop_create_default();

  wifi_connect();
}
