#include <stm32f4xx.h>
#include "utils.h"

/* For SysTick */
static __IO uint32_t TimingDelay;

void Delay(__IO uint32_t nTime){
	TimingDelay = nTime;

	while(TimingDelay != 0);
}

void TimingDelay_Decrement(void){
	if (TimingDelay != 0x00){ 
		TimingDelay--;
	}
}
