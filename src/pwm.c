#include <stm32f4xx_gpio.h>
#include <stm32f4xx_tim.h>

#include "pwm.h"
#include "utils.h"

static void pwm_init_TIM_pins(void){
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable GPIOD */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Set the AF */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);
	
	/* Configure GPIOD pins for PWM Channels */
	GPIO_InitStruct.GPIO_Pin = ( GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);

	//GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13);
}

static void pwm_init_TIM4_timing(void){
	
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	/* Enable TIM4 clk */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* TIM4 timing settings */
	TIM_InitStruct.TIM_Prescaler = 0;
	TIM_InitStruct.TIM_Period = 8399;
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_InitStruct);

	TIM_ARRPreloadConfig(TIM4, ENABLE);	

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC1Init(TIM4, &TIM_OCInitStruct);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
		
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	
	TIM_OC2Init(TIM4, &TIM_OCInitStruct);
	
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	
	TIM_OC3Init(TIM4, &TIM_OCInitStruct);
	
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	
	TIM_OC4Init(TIM4, &TIM_OCInitStruct);
	
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	
	/* TIM4 enable timer */
	TIM_Cmd(TIM4, ENABLE);
}

void pwm_init_pwm(void){
	pwm_init_TIM_pins();
	pwm_init_TIM4_timing();
}

void pwm_set_compare1(uint16_t pwm_val){
	TIM_SetCompare1(TIM4, (uint32_t) pwm_val);
}

void pwm_set_compare2(uint16_t pwm_val){
	TIM_SetCompare2(TIM4, (uint32_t) pwm_val);
}

void pwm_set_compare3(uint16_t pwm_val){
	TIM_SetCompare3(TIM4, (uint32_t) pwm_val);
}

void pwm_set_compare4(uint16_t pwm_val){
	TIM_SetCompare4(TIM4, (uint32_t) pwm_val);
}
