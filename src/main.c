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


#define BETA 0.3
#define SENSITIVITY 0.00875
#define DT 0.02;

volatile uint8_t pwm_val = 0;
volatile uint8_t gyro = 0;

static float heading = 0;
static int16_t gyro_x = 0;
static int16_t gyro_y = 0;
static float gyro_calibration_tmp = 0;
static float gyro_x_offset = 0;
static float raw_gyro = 0;
static float angle = 0.0;

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
	//gpio_setup_gpio();
	//pwm_init_pwm();
	//acc_init_acc();
	//acc_init_mag(0); /* init mag but not the temperature sensor */
	if(gyro_init_gyro()){
		while(1);
	}

	for(int i = 0; i < 100; i++){
 		gyro_calibration_tmp += gyro_get_X() * SENSITIVITY;
		DEBUG("GYRO(X = [%g])", gyro_calibration_tmp);
 	}
	
 	gyro_x_offset = gyro_calibration_tmp/100.0;
	DEBUG("GYRO OFFSET = [%g]", gyro_x_offset);
        
	while (1){

		/* Lets first start from calibrating the mmeter */
		//acc_mag_calibrate();
		
	        //update_PWM();
		if(gyro){
			raw_gyro = gyro_get_X() * SENSITIVITY;
			
			angle += raw_gyro * DT;
			
 			DEBUG("GYRO(X = [%g], raw = [%g])", angle, raw_gyro);
 		}
		//update_PWM();
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
