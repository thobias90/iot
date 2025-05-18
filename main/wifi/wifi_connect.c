#include "wifi_connect.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#define EXAMPLE_NETIF_DESC_STA "example_netif_sta"
static const char *TAG = "example_connect";
static esp_netif_t *s_example_sta_netif = NULL;

static void example_handler_on_sta_got_ip(void *arg,
                                          esp_event_base_t event_base,
                                          int32_t event_id, void *event_data) {
  ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
  ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR,
           esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
  ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
}

void wifi_start(void) {
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_netif_inherent_config_t esp_netif_config =
      ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
  esp_netif_config.if_desc = EXAMPLE_NETIF_DESC_STA;
  esp_netif_config.route_prio = 128;
  s_example_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);
  esp_wifi_set_default_wifi_sta_handlers();

  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
}

esp_err_t wifi_connect(void) {
  ESP_LOGI(TAG, "Connecting to Wi-Fi...");
  wifi_start();
  wifi_config_t wifi_config = {
      .sta =
          {
              .ssid = "your-network-ssid",
              .password = "your-network-password",
              .scan_method = WIFI_FAST_SCAN,
              .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
              // .threshold.rssi = CONFIG_EXAMPLE_WIFI_SCAN_RSSI_THRESHOLD,
              // .threshold.authmode = EXAMPLE_WIFI_SCAN_AUTH_MODE_THRESHOLD,
          },
  };
  esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                             &example_handler_on_sta_got_ip, NULL);
  ESP_LOGI(TAG, "Connecting to %s...", wifi_config.sta.ssid);
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  esp_err_t ret = esp_wifi_connect();
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
    return ret;
  }
  return ESP_OK;
}