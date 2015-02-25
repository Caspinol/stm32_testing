#include "systick.h"
#include "utils.h"
#include "gpio.h"

/* led flashing mode */
int modeFlag = 0;

int main(){
    
  SysTickInit();
  qc_gpio_init();
  //qc_i2c_init();
  
  while(1){
    
    qc_gpio_basic_flash(SET_ms(250));
    while(modeFlag){
      qc_gpio_basic_flash(SET_ms(50));
    }
  }

  return 0;
}
