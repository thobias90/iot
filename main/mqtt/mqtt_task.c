#include "mqtt_task.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "events.h"
#include "shared.h"

static const char* TAG = "MQTT";
static esp_mqtt_client_handle_t mqtt_client = NULL;

static void mqtt_event_data(char* topic, int topic_len, char* data, int data_len) {
  ESP_LOGI(TAG, "MQTT_EVENT_DATA");
  printf("TOPIC=%.*s\r\n", topic_len, topic);
  printf("DATA=%.*s\r\n", data_len, data);
  int led_request;
  if (*data == '1') {
    // esp_mqtt_client_publish(mqtt_client, "/button-state", "{\"on\": true}", 12, 0, 0);
    led_request = 1;
    xQueueSend(s_led_requests, &led_request, 0);
  } else {
    led_request = 0;
    xQueueSend(s_led_requests, &led_request, 0);
  }
}


static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
  ESP_LOGI(TAG, "Handling MQTT event, event loop base=%s, event_id=%" PRIi32, base, event_id);
  esp_mqtt_event_handle_t event = event_data;
  esp_mqtt_client_handle_t client = event->client;
  int msg_id;
  switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
      msg_id = esp_mqtt_client_subscribe(client, "/led/cmd", 0);
      ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      break;

    case MQTT_EVENT_SUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_UNSUBSCRIBED:
      ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_DATA:
      mqtt_event_data(event->topic, event->topic_len, event->data, event->data_len);
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
      if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
        ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
      }
      break;
    default:
      ESP_LOGI(TAG, "Other event id:%d", event->event_id);
      break;
  }
}
static void mqtt_app_start(void) {
  const esp_mqtt_client_config_t mqtt_cfg = {
      .broker.address.uri = BROKER_URI,
  };

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
  mqtt_client = client;
  /* The last argument may be used to pass data to the event handler, in this
   * example mqtt_event_handler */
  esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
  esp_mqtt_client_start(client);
}


void mqtt_task(void* param) {
  ESP_LOGI(TAG, "Starting MQTT task...");
  EventBits_t bits = xEventGroupWaitBits(s_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  if (bits & WIFI_CONNECTED_BIT) {
    ESP_LOGI(TAG, "MQTT task: WiFi connected");
  } else {
    ESP_LOGI(TAG, "MQTT task: WiFi not connected");
    vTaskDelete(NULL);
  }
  mqtt_app_start();

  int led_state;

  while (1) {
    if (xQueueReceive(s_led_events, &led_state, portMAX_DELAY)) {
      ESP_LOGI(TAG, "LED state: %d", led_state);
      if (led_state) {
        esp_mqtt_client_publish(mqtt_client, "/led/state", "1", 1, 0, 0);
      } else {
        esp_mqtt_client_publish(mqtt_client, "/led/state", "0", 1, 0, 0);
      }
    }
  }
}