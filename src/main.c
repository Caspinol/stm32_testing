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

#define DT 0.02
#define BETA 0.20

volatile uint_fast8_t pwm_val = 0,
	gyro = 0;

static float heading = 0;

static float angle_x = 0.0,
	angle_y = 0.0,
	angle_z = 0.0;

static gyro_xyz_t g_xyz;
static acc_angle_t acc_angle;

//void update_PWM(void);

int main(void){
	
	/* 
	   Setup Systick Timer every 1ms
	*/
	if (SysTick_Config(SystemCoreClock / 1000)){ 
		/* Capture error */ 
		while(1);
	}

	/* Enable GPIO stuff (LEDs and button) */
	gpio_setup_gpio();
	tim_init_tim3();
	//pwm_init_pwm();
	acc_init_acc();
	acc_init_mag(0); /* init mag but not the temperature sensor */
	if(gyro_init_gyro()){
		while(1);
	}
	
	while (1){
		
		if(gyro){
			/* We gonna attempt a little complementary filter here */
			gyro_get_xyz(&g_xyz);
			acc_get_acc_angle(&acc_angle);

			//DEBUG("ACCELERO_ANGLE(Pitch = [%g], Roll = [%g])",
			//      acc_angle.pitch, acc_angle.roll);

			angle_x = BETA * (angle_x + g_xyz.x * DT) + (1.0 - BETA) * acc_angle.pitch;
			angle_y = BETA * (angle_y + g_xyz.y * DT) + (1.0 - BETA) * acc_angle.roll;
			
 			DEBUG("ANGLE (Roll = [%g], Pitch = [%g])",
			      angle_x, angle_y);
			gyro = 0;
 		}

		//heading = acc_get_heading();

		//DEBUG("Heading = [%g]", heading);
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
