#include "systick.h"
#include "utils.h"
#include "gpio.h"

/* led flashing mode */
int modeFlag = 0;

int main(){
    
  kg_systick_init();
  kg_gpio_init();

  while(1){
    
    kg_gpio_basic_flash(SET_ms(250));
    while(modeFlag){
      kg_gpio_basic_flash(SET_ms(50));
    }
  }

  return 0;
}
