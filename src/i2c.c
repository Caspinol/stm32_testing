#include <stm32f401xc.h>
#include "utils.h"
#include "i2c.h"

/*
 *This i2c driver will be needed to communicate with the 
 *LSM303  accelometer/magnetometer
 *PINOUT:
        SCL - PB6
        SDA - PB9
        INT1 - PE5
        INT2 - PE4
*/

void qc_i2c_init(){
  /* Init GPIOB clock */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  /* enable clock for i2c1 peripheral */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  /* configure the GPIOB pin 6 adn 9 as AF4 */
  GPIOB->AFR[0] |= 0x04000000;
  GPIOB->AFR[1] |= 0x00000040;
  /* open drain */
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_9);
  
}

