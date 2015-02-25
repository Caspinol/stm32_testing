#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include <stm32f401xc.h>

void SysTickInit();
void SysTickIncrement();
uint32_t SysTickGetTick();


#endif
