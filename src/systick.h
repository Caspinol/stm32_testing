#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include <stm32f401xc.h>

void kg_systick_init();
void kg_systick_increment();
uint32_t kg_systick_get_tick();


#endif
