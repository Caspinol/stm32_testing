#include "gpio.h"

/* led flashing mode */
int modeFlag = 0;

int main(){
    
  qc_gpio_init();
  //qc_i2c_init();
  
  while(1){
    
    qc_gpio_basic_flash(500000);
    while(modeFlag){
      qc_gpio_basic_flash(100000);
    }
  }

  return 0;
}
