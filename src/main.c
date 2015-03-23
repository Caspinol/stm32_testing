#include "systick.h"
#include "utils.h"
#include "gpio.h"
#include "i2c.h"

#define ACCELO_ADDR_WRITE 0x32
#define ACCELO_ADDR_READ 0x33
#define ACCELO_IRB_REG_M 0x0b

/* led flashing mode */
int modeFlag = 1; /*start in flash mode*/

int main(){
    
  int8 buf[5];

  kg_systick_init();
  kg_gpio_init();
  qc_i2c_init();


  while(1){
    
    kg_gpio_basic_flash(SET_ms(250));
    while(modeFlag){
      kg_gpio_basic_flash(SET_ms(50));
    }

    qc_i2c_read(I2C1, ACCELO_ADDR_READ, ACCELO_IRB_REG_M, buf, 1);
  }

  return 0;
}
