#ifndef GPIO_TASK_H
#define GPIO_TASK_H

#define GPIO_OUTPUT 25
#define GPIO_OUTPUT_PIN_SEL 1ULL << GPIO_OUTPUT

#define GPIO_INPUT 21
#define GPIO_INPUT_PIN_SEL 1ULL << GPIO_INPUT

void gpio_task(void *param);

#endif // GPIO_TASK_H