#include <stm32f401xc.h>
#include "utils.h"
#include "systick.h"


void qc_delay(uint32_t us){
  uint32_t tick = SysTickGetTick();
  while((SysTickGetTick() - tick) <= us);
}
