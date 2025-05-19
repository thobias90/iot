#include "gpio_task.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "shared.h"

#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_TIME_US 20000

static QueueHandle_t gpio_evt_queue = NULL;
static esp_timer_handle_t debounce_timer = NULL;
static volatile bool is_debouncing = false;

static const char* TAG = "GPIO";

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  if (!is_debouncing) {
    is_debouncing = true;
    esp_timer_start_once(debounce_timer, DEBOUNCE_TIME_US);
  }
}

static void IRAM_ATTR debounce_timer_callback(void* arg) {
  uint32_t gpio_num = (uint32_t)arg;
  if (gpio_get_level(gpio_num) == 1) {
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
  }
  is_debouncing = false;
}

static void gpio_init(void) {
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);
}

static void gpio_interrupt_config(void) {
  gpio_set_intr_type(GPIO_INPUT, GPIO_INTR_POSEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add(GPIO_INPUT, gpio_isr_handler, (void*)GPIO_INPUT);
}

static void timer_init(void) {
  const esp_timer_create_args_t timer = {.callback = &debounce_timer_callback,
                                         .dispatch_method = ESP_TIMER_ISR,
                                         .arg = (void*)GPIO_INPUT,
                                         .name = "debounce_timer"};
  ESP_ERROR_CHECK(esp_timer_create(&timer, &debounce_timer));
}

void gpio_task(void* param) {
  ESP_LOGI(TAG, "GPIO Task Started..");

  uint32_t io_num;
  int led_state = 0;
  int led_request;

  timer_init();
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  gpio_init();
  gpio_interrupt_config();

  while (1) {
    if (xQueueReceive(gpio_evt_queue, &io_num, 0)) {
      if (io_num == GPIO_INPUT) {
        led_state = !led_state;
        gpio_set_level(GPIO_OUTPUT, led_state);
        xQueueSend(s_led_events, &led_state, 0);
      }
    }
    if (xQueueReceive(s_led_requests, &led_request, 0)) {
      if (led_request) {
        gpio_set_level(GPIO_OUTPUT, 1);
      } else {
        gpio_set_level(GPIO_OUTPUT, 0);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
