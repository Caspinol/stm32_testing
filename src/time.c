#include <stm32f4xx.h>
#include "time.h"

/* For SysTick */
static __IO uint32_t systick_heartbeat;

void Delay(__IO uint32_t time){
	systick_heartbeat = time;

	while(systick_heartbeat != 0);
}

void update_systick(void){
	if (systick_heartbeat != 0x00){ 
		systick_heartbeat--;
	}
}

uint32_t get_systick(void){
	return systick_heartbeat;
}
