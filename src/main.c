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

#define DT 0.02;

volatile uint8_t pwm_val = 0,
	gyro = 0;

static float heading = 0;

static float angle_x = 0.0,
	angle_y = 0.0,
	angle_z = 0.0;
static struct gyro_xyz_t g_xyz;

//void update_PWM(void);

int main(void){
	
	/* 
	   Setup Systick Timer every 1ms
	*/
	if (SysTick_Config(SystemCoreClock / 1000)){ 
		/* Capture error */ 
		DEBUG("SysTick initialization error");
		while(1);
	}

	/* Enable GPIO stuff (LEDs and button) */
	gpio_setup_gpio();
	tim_init_tim3();
	//pwm_init_pwm();
	//acc_init_acc();
	//acc_init_mag(0); /* init mag but not the temperature sensor */
	if(gyro_init_gyro()){
		while(1);
	}
	
	while (1){

		/* Lets first start from calibrating the mmeter */
		//acc_mag_calibrate();
		
	        //update_PWM();
		if(gyro){
			gyro_get_xyz(&g_xyz);
			
			angle_x += g_xyz.x * DT;
			angle_y += g_xyz.y * DT;
			angle_z += g_xyz.z * DT;
			
 			DEBUG("GYRO(X = [%g], Y = [%g], Z = [%g])",
			      angle_x*10, angle_y*10, angle_z*10);
			gyro = 0;
 		}
	}
}

#if 0
void update_PWM(void){
	pwm_set_compare1(PWM_SET_DUTY(pwm_val));
	pwm_set_compare2(PWM_SET_DUTY(pwm_val));
	pwm_set_compare3(PWM_SET_DUTY(pwm_val));
	pwm_set_compare4(PWM_SET_DUTY(pwm_val));
}
#endif
