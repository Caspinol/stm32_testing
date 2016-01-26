#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stdio.h>

#include "gpio.h"
#include "pwm.h"
#include "accelero.h"

#include "utils.h"

uint8_t pwm_val = 0;
int16_t out_x = 0;
int16_t out_y = 0;
int16_t out_z = 0;

void update_PWM(void);

int main(void){
	
	/* 
	   Setup Systick Timer every ~1ms
	   SysTick
	*/
	if (SysTick_Config(SystemCoreClock / 1000)){ 
		/* Capture error */ 
		while (1);
	}

	/* Enable GPIO stuff (LEDs and button) */
	gpio_setup_gpio();
	pwm_init_pwm();
	acc_init_acc();
	
	while (1){
		
	        update_PWM();
		
		out_x = acc_get_acc_x();
		DEBUG("x = [%d]", out_x);

		out_y = acc_get_acc_y();
		DEBUG("y = [%d]", out_y);

		out_z = acc_get_acc_z();
		DEBUG("z = [%d]", out_z);

		Delay(2000);
	}
}

void update_PWM(void){
	pwm_set_compare1(PWM_SET_DUTY(pwm_val));
	pwm_set_compare2(PWM_SET_DUTY(pwm_val));
	pwm_set_compare3(PWM_SET_DUTY(pwm_val));
	pwm_set_compare4(PWM_SET_DUTY(pwm_val));
}
