#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stdio.h>

#include "gpio.h"
#include "pwm.h"
#include "utils.h"

#define PWM_SET_DUTY(DUTY) (((8399 + 1) * (DUTY)) / 100 - 1)

int8 pwm_val = 0;

int main(void){
	
	/* 
	   Setup Systick Timer every ~1ms
	   SysTick
	*/
	if (SysTick_Config(SystemCoreClock / 100)){ 
		/* Capture error */ 
		while (1);
	}

	/* Enable GPIO stuff (LEDs and button) */
	gpio_setup_gpio();
	pwm_init_pwm();
	while (1){
		
		printf("The PWM duty is %d\n",pwm_val);
		printf("The PWM val is %d\n",PWM_SET_DUTY(pwm_val));
		pwm_set_compare1(PWM_SET_DUTY(pwm_val));
		pwm_set_compare2(PWM_SET_DUTY(pwm_val));
		pwm_set_compare3(PWM_SET_DUTY(pwm_val));
		pwm_set_compare4(PWM_SET_DUTY(pwm_val));
	}
}
