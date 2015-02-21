#include <stm32f401xc.h>
#include "utils.h"


void qc_delay(uint32_t d){
  volatile int i,j;
  
  for (i=0; i < d; i++){
    j++;
  }
}
