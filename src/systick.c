#include <stm32f401xc.h>
#include "systick.h"

static volatile uint32_t the_tick = 0;

void kg_systick_init(){
  
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

void kg_systick_increment(){
  the_tick++;
}

uint32_t kg_systick_get_tick(){
  return the_tick;
}
