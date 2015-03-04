#include <stm32f401xc.h>
#include "gpio.h"
#include "utils.h"

#define LED_GREEN GPIO_PIN_12
#define LED_ORANGE GPIO_PIN_13
#define LED_RED GPIO_PIN_14
#define LED_BLUE GPIO_PIN_15

void kg_gpio_init(){

  /* GPIO D and A Periph clock enable */
  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN);
  
  /* set GPIOD pins 12, 13, 14 and 15 as output */
  GPIOD->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);
  /* and GPIOA pin 0 as input*/
  GPIOA->MODER &= ~GPIO_MODER_MODER0;

  /* push-pull output */
  GPIOD->OTYPER &= ~(GPIO_OTYPER_OT_12 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_15);
  
  /* max speed */
  GPIOD->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;

  /* outputs so disable pull_xxx */
  GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR12 | GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14 | GPIO_PUPDR_PUPDR15);
  /* GPIoA pull-down enabled */
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;

  /* now the button interrupt settings */
  SYSCFG->EXTICR[0] &= SYSCFG_EXTICR1_EXTI0_PA; /* select GPIOA pin0 */
  EXTI->RTSR |= EXTI_RTSR_TR0; /* rising edge enable */
  EXTI->IMR |= EXTI_IMR_MR0; /* enable interrupt */
  /* set NVIC priority to 1 */
  NVIC_SetPriority(EXTI0_IRQn, 1);
  /* enable */
  NVIC_EnableIRQ(EXTI0_IRQn);
}

void kg_gpio_pin_set(GPIO_TypeDef *GPIOPort, uint16_t pin){
  /* set pin in the lower 16bit*/
  GPIOPort->BSRR |= pin;
}

void kg_gpio_pin_clear(GPIO_TypeDef *GPIOPort, uint16_t pin){
  /* set in the higher 16bit */
  GPIOPort->BSRR |= (pin << 16);
}

int kg_gpio_pin_get(GPIO_TypeDef *GPIOPort, uint16_t pin){
  
  int ret = 0;
  if(GPIOPort->IDR & pin){
    kg_delay(10000);
    if(GPIOPort->IDR & pin){
      ret = 1;
    }
  }
  return ret;
}

void kg_gpio_port_set(GPIO_TypeDef *GPIOPort){

}

void kg_gpio_port_read(GPIO_TypeDef *GPIOPort){

}

void kg_gpio_basic_flash(uint32_t d){
  kg_gpio_pin_clear(GPIOD, LED_GREEN);
  kg_gpio_pin_set(GPIOD, LED_ORANGE); 
  kg_gpio_pin_clear(GPIOD, LED_RED);
  kg_gpio_pin_set(GPIOD, LED_BLUE);
  
  kg_delay(d);
  
  kg_gpio_pin_set(GPIOD, LED_GREEN);
  kg_gpio_pin_clear(GPIOD, LED_ORANGE);
  kg_gpio_pin_set(GPIOD, LED_RED);
  kg_gpio_pin_clear(GPIOD, LED_BLUE);

  kg_delay(d);
}
