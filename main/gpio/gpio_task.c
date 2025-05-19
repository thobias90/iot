#include "gpio_task.h"

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define ESP_INTR_FLAG_DEFAULT 0

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  uint32_t gpio_num = (uint32_t)arg;
  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
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
  gpio_set_intr_type(GPIO_INPUT, GPIO_INTR_ANYEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add(GPIO_INPUT, gpio_isr_handler, (void*)GPIO_INPUT);
}

void gpio_task(void* param) {
  uint32_t io_num;
  bool led_state = false;
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  gpio_init();
  gpio_interrupt_config();

  while (1) {
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
      if (io_num == GPIO_INPUT) {
        led_state = !led_state;
        gpio_set_level(GPIO_OUTPUT, led_state);
      }
    }
  }
}
