#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stdio.h>

#include "time.h"
#include "utils.h"
#include "gpio.h"
#include "pwm.h"
#include "accelero.h"
#include "gyro.h"

uint8_t pwm_val = 0;

static float heading = 0;
static int16_t gyro_x = 0;
static int16_t gyro_y = 0;

void update_PWM(void);

int main(void){
	
	/* 
	   Setup Systick Timer every ~1ms
	*/
	if (SysTick_Config(SystemCoreClock / 1000)){ 
		/* Capture error */ 
		DEBUG("SysTick initialization error");
		while(1);
	}

	/* Enable GPIO stuff (LEDs and button) */
	gpio_setup_gpio();
	pwm_init_pwm();
	acc_init_acc();
	acc_init_mag(0); /* init mag but not the temperature sensor */
	if(gyro_init_gyro()){
		while(1);
	}
        
	while (1){

		/* Lets first start from calibrating the mmeter */
		acc_mag_calibrate();
		
	        //update_PWM();
		heading = acc_get_heading();
		DEBUG("Heading = [%g]", heading);

		gyro_x = gyro_get_X();
		gyro_y = gyro_get_Y();

		DEBUG("GYRO(X = [%d], Y = [%d])",gyro_x, gyro_y);
		Delay(1500);
		update_PWM();
	}
}

void update_PWM(void){
	pwm_set_compare1(PWM_SET_DUTY(pwm_val));
	pwm_set_compare2(PWM_SET_DUTY(pwm_val));
	pwm_set_compare3(PWM_SET_DUTY(pwm_val));
	pwm_set_compare4(PWM_SET_DUTY(pwm_val));
}
