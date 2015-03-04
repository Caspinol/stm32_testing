#include <stm32f401xc.h>
#include "utils.h"
#include "systick.h"


void kg_delay(uint32_t us){
  uint32_t tick = kg_systick_get_tick();
  while((kg_systick_get_tick() - tick) <= us);
}
