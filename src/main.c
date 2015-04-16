#include "systick.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"
#include "pwm.h"

#define ACCELO_ADDR_WRITE 0x32
#define ACCELO_ADDR_READ 0x33
#define MAGNET_ADDR_WRITE 0x3d
#define MAGNET_ADDR_READ 0x3c

#define WHO_AM_I_ADDR 0x0F

#define ACCELO_IRB_REG_M 0x0b

/* counter value */
int pwm_val = 0;

int main(){
	
	kg_systick_init();
	kg_gpio_init();
	//qc_i2c_init();
	kg_pwm_init();
	
	
	while(1){

		if(pwm_val > 40) pwm_val = 0;
	
		PWM_SET_CCR_CH1(TIM4, pwm_val);
		PWM_SET_CCR_CH2(TIM4, pwm_val + 10);
		PWM_SET_CCR_CH3(TIM4, pwm_val - 5);
		PWM_SET_CCR_CH4(TIM4, pwm_val - 15);

		//qc_i2c_read(I2C1, ACCELO_ADDR_READ, WHO_AM_I_ADDR, &buf, 1);
		
		//qc_i2c_read(I2C1, MAGNET_ADDR_READ, WHO_AM_I_ADDR, &buf, 1);
	}
	
	return 0;
}
