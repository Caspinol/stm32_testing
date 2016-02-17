#ifndef __GPIO_H__
#define __GPIO_H__

#define LED_GREEN GPIO_Pin_12
#define LED_YELLOW GPIO_Pin_13
#define LED_RED GPIO_Pin_14
#define LED_BLUE GPIO_Pin_15

void gpio_setup_gpio(void);
void gpio_do_flash(void);
void gpio_LED_ON(uint16_t led);
void gpio_LED_OFF(uint16_t led);
void gpio_LED_TOGGLE(uint16_t led);

#endif
