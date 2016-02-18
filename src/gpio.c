#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_syscfg.h>

#include "time.h"
#include "utils.h"
#include "gpio.h"

#define D2s 2000 // delay of 2 seconds

static void gpio_setup_LEDs(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* GPIOD Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	/* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
	GPIO_InitStruct.GPIO_Pin = LED_GREEN | LED_RED | LED_BLUE | LED_YELLOW;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/*
  Sets the button on PA0 and the EXTI for it
 */
static void gpio_setup_Button(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	/* GPIOA Periph clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* and PA0 ans a button input */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Set the external interrupt */

	/* Enable SYSCFG */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Set PA0 as EXTI interrupt */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	/* Configure EXTI */
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	/* Configure NVIC for EXTI */
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void gpio_setup_gpio(void){
	gpio_setup_LEDs();
	gpio_setup_Button();
}

void gpio_do_flash(void){
	gpio_LED_ON(LED_GREEN);
	Delay(D2s);
	gpio_LED_ON(LED_YELLOW);
	Delay(D2s);
	gpio_LED_ON(LED_RED);
	Delay(D2s);
	gpio_LED_ON(LED_BLUE);
	Delay(5000);
	gpio_LED_OFF(LED_GREEN | LED_YELLOW | LED_RED | LED_BLUE);
	Delay(10000);
}

void gpio_LED_ON(uint16_t led){
	GPIO_SetBits(GPIOD, led);
}

void gpio_LED_OFF(uint16_t led){
	GPIO_ResetBits(GPIOD, led);
}

void gpio_LED_TOGGLE(uint16_t led){
        GPIO_ToggleBits(GPIOD, led);
}
