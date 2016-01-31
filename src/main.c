#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_exti.h>
#include <stdio.h>

#include "utils.h"
#include "gpio.h"
#include "pwm.h"
#include "accelero.h"

#include <math.h>
//#include <arm_math.h>

//#define M_PI 3.14159265358979323846
#define M_PI 3.14159

#define CALIBRATION_DELAY 200 // ms
#define CALIBRATION_DURATION (10000 / CALIBRATION_DELAY)

#define DEG_TO_RAD(DEG) ((DEG) * M_PI / 180.0)
#define RAD_TO_DEG(RAD) ((RAD) * 180.0 / M_PI)

static uint8_t calibration_done = 0;

uint8_t pwm_val = 0;

static int16_t a_out[3] = {0}; /* Stor 3 axis values for acc */
static int16_t m_out[3] = {0}; /* same but for mag */

static float heading = 0;
static float pitch = 0;
static float roll = 0;

/* Used for calibration */
static int16_t mag_x_max = 0,
	mag_x_min = 0,
	mag_y_max = 0,
	mag_y_min = 0,
	mag_z_max = 0,
	mag_z_min = 0;

static float MAG_Xc = 0;
static float MAG_Yc = 0;
static float MAG_Zc = 0;
static float MAG_Xh = 0;
static float MAG_Yh = 0;

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
	//pwm_init_pwm();
	acc_init_acc();
	acc_init_mag(0); /* init mag but not the temperature sensor */

        
	while (1){

		/* Lets first start from calibrating the mmeter */
		if(!calibration_done){
			for(int i = 0; i < CALIBRATION_DURATION; i++){
				
				DEBUG("Calibrating...");
				acc_get_mag_xyz(m_out, 3);
				
				if(m_out[MX_AXIS] < mag_x_min) mag_x_min = m_out[MX_AXIS];
				if(m_out[MX_AXIS] > mag_x_max) mag_x_max = m_out[MX_AXIS];
				
				if(m_out[MZ_AXIS] < mag_z_min) mag_z_min = m_out[MZ_AXIS];
				if(m_out[MZ_AXIS] > mag_z_max) mag_z_max = m_out[MZ_AXIS];
				
				if(m_out[MY_AXIS] < mag_y_min) mag_y_min = m_out[MY_AXIS];
				if(m_out[MY_AXIS] > mag_y_max) mag_y_max = m_out[MY_AXIS];
				
				DEBUG("Current:\n\tMAX X = [%d]\n\tMIN X = [%d]\n\tMAX Y = [%d]\n\tMIN Y = [%d]\n\t"
				      "MAX Z = [%d]\n\tMIN Z = [%d]",
				      mag_x_max, mag_x_min, mag_y_max, mag_y_min, mag_z_max, mag_z_min);

				Delay(CALIBRATION_DELAY);
			}
			
			calibration_done = 1;
		}
		
	        //update_PWM();
		a_out[AX_AXIS] = acc_get_acc_x();
		a_out[AY_AXIS] = acc_get_acc_y();
		a_out[AZ_AXIS] = acc_get_acc_z();

		DEBUG("\nAX = [%d], AY = [%d], AZ = [%d]\n", a_out[AX_AXIS], a_out[AY_AXIS], a_out[AZ_AXIS]);
		/* Equation 40 */
		float norAX = (float)(a_out[AX_AXIS]/sqrt(a_out[AX_AXIS]*a_out[AX_AXIS]+
							  a_out[AY_AXIS]*a_out[AY_AXIS]+
							  a_out[AZ_AXIS]*a_out[AZ_AXIS]));
		
		float norAY = (float)(a_out[AY_AXIS]/sqrt(a_out[AX_AXIS]*a_out[AX_AXIS]+
							  a_out[AY_AXIS]*a_out[AY_AXIS]+
							  a_out[AZ_AXIS]*a_out[AZ_AXIS]));
		/*
		float norAZ = (float)(a_out[AZ_AXIS]/sqrt(a_out[AX_AXIS]*a_out[AX_AXIS]+
							  a_out[AY_AXIS]*a_out[AY_AXIS]+
							  a_out[AZ_AXIS]*a_out[AZ_AXIS]));
		*/
		/* Equation 10 */
		pitch = asin(-norAX);
		//pitch = RAD_TO_DEG(atan2(norAY, sqrt(norAX * norAX + norAZ * norAZ)));
		float c_pitch = cos(pitch);
		float s_pitch = sin(pitch);

		roll = asin(norAY/c_pitch);
		//roll = RAD_TO_DEG(atan2(-norAX, norAZ));
		float s_roll = sin(roll);
		float c_roll = cos(roll);
		
		
		acc_get_mag_xyz(m_out, 3);
		
		/* 
		   Calculate the offset based on the min max values
		   Equation 12
		 */
		MAG_Xc = (float)(m_out[MX_AXIS] - mag_x_min) / (mag_x_max - mag_x_min) * 2 - 1;
		MAG_Yc = (float)(m_out[MY_AXIS] - mag_y_min) / (mag_y_max - mag_y_min) * 2 - 1;
		MAG_Zc = (float)(m_out[MZ_AXIS] - mag_z_min) / (mag_z_max - mag_z_min) * 2 - 1;

		MAG_Xh = MAG_Xc * c_pitch + MAG_Zc * s_pitch;
		MAG_Yh = MAG_Xc * s_roll * s_pitch + MAG_Yc * c_roll - MAG_Zc * s_roll * c_pitch;

		/* Now we can find out the heading */
		heading = RAD_TO_DEG(atan2(MAG_Yh, MAG_Xh));
		
		if(heading < 0){
			heading += 360;
		}
		
		DEBUG("Heading = [%g]\nPitch = [%g]\nRoll = [%g]", heading, roll, pitch);

		Delay(2500);
		
	}
}

void update_PWM(void){
	pwm_set_compare1(PWM_SET_DUTY(pwm_val));
	pwm_set_compare2(PWM_SET_DUTY(pwm_val));
	pwm_set_compare3(PWM_SET_DUTY(pwm_val));
	pwm_set_compare4(PWM_SET_DUTY(pwm_val));
}
