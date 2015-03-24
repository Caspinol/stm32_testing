#include "systick.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"

#define ACCELO_ADDR_WRITE 0x32
#define ACCELO_ADDR_READ 0x33
#define MAGNET_ADDR_WRITE 0x3d
#define MAGNET_ADDR_READ 0x3c

#define WHO_AM_I_ADDR 0x0F

#define ACCELO_IRB_REG_M 0x0b

/* led flashing mode */
int modeFlag = 1; /*start in flash mode*/

int main(){
    
	int8 buf = 0;
	
	kg_systick_init();
	kg_gpio_init();
	qc_i2c_init();
	
	
	while(1){
		
		kg_gpio_basic_flash(SET_ms(250));
		while(modeFlag){
			kg_gpio_basic_flash(SET_ms(50));
		}
		
		qc_i2c_read(I2C1, ACCELO_ADDR_READ, WHO_AM_I_ADDR, &buf, 1);
		
		qc_i2c_read(I2C1, MAGNET_ADDR_READ, WHO_AM_I_ADDR, &buf, 1);
		
		(void)buf;
	}
	
	return 0;
}
