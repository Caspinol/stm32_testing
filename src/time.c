#include <stm32f4xx.h>
#include "time.h"

/* For SysTick */
static __IO uint_fast32_t systick_heartbeat;

void Delay(uint_fast32_t const time){
	systick_heartbeat = time;

	while(systick_heartbeat != 0);
}

void update_systick(void){
	if (systick_heartbeat != 0x00){ 
		systick_heartbeat--;
	}
}

uint_fast32_t get_systick(void){
	return systick_heartbeat;
}

void tim_init_tim3(void){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	/* Enable TIM3 clk. 
	   Apparently its better to do it after the interrupt config 
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* TIM3 timing settings 
	   We need interrupt firing every 20ms
	*/
	TIM_InitStruct.TIM_Prescaler = 8400 - 1;
	TIM_InitStruct.TIM_Period = 200 - 1;
	TIM_InitStruct.TIM_ClockDivision = 0;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_InitStruct);

	/* Enable interrupts */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	/* TIM3 enable timer */
	TIM_Cmd(TIM3, ENABLE);
}
