#include <stm32f4xx.h>
#include <stm32f4xx_gpio.h>

#include "interrupts.h"

#include "utils.h"

extern volatile int8 pwm_val;

void NMI_Handler(void)
{
}

void HardFault_Handler(void)
{
  while (1)
  { }
}

void MemManage_Handler(void)
{
  while (1)
  { }
}

void BusFault_Handler(void)
{
  while (1)
  { }
}

void UsageFault_Handler(void)
{
  while (1)
  { }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){		
		if(pwm_val >= 100){
			pwm_val = 0;
		}
		pwm_val += 2;

		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

