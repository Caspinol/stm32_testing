#include <stm32f401xc.h>
#include "systick.h"

static volatile uint32_t theTick = 0;

void SysTickInit(){
  
  /* 1 processor tick with AHB/8 occurs 
     every 100ns but I want 1us ticks
     so load it with 10-1
  */
  SysTick->LOAD |= (9 << 0);

  SysTick->VAL = 0;

  /* AHB/8 int enable */
  SysTick->CTRL &= 0;
  SysTick->CTRL = (3 << 0);

  /* set NVIC priority to 1 */
  NVIC_SetPriority(SysTick_IRQn, 0);
  /* enable */
  NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTickIncrement(){
  theTick++;
}

uint32_t SysTickGetTick(){
  return theTick;
}
