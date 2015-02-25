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

#define I2C_START(I2CX)   (I2CX->CR1 |= I2C_CR1_START)
#define I2C_STOP(I2CX)   (I2CX->CR1 |= I2C_CR1_STOP)
#define I2C_CHECK_FLAG() ()

void qc_i2c_init(){
  /* Init GPIOB and GPIOE clock */
  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOEEN);
  /* enable clock for i2c1 peripheral */
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  /* configure the GPIOB pin 6 adn 9 as AF4 */
  GPIOB->AFR[0] |= 0x04000000;
  GPIOB->AFR[1] |= 0x00000040;
  /* open drain for the i2c pins*/
  GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_9);
  /* and pin PE5 as interrupt input */
  GPIOE->MODER &= ~GPIO_MODER_MODER5;
  /* max speed */
  GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
  /* GPIOE pull-down enabled */
  GPIOE->PUPDR |= GPIO_PUPDR_PUPDR5_1;
  /*
    TODO: set the interrupt config - maybe
   */
  
  /* clock is 40MHz */
  I2C1->CR2 |= (40 << 0);

  /* 7bit addressing */
  I2C1->OAR1 |= ~(I2C_OAR1_ADDMODE);
  /* use only OAR1 */
  I2C1->OAR2 &= 0x00000000;
  
  /*Fm mode and 16/9 duty cycle */
  I2C1->CCR |= (I2C_CCR_FS | I2C_CCR_DUTY);

  /* 
     lets set it for 400kHz therefore the CCR:
     1/400kHz = 2.5us = T = Thigh + Tlow
     Tpclk = 1/40MHz = 25ns
     Thigh = 9 * CCR * Tpclk
     Tlow = 16 * CCR * Tplck =>
     => 2.5us = 25 * CCR * 25ns
     CRR = 2.5us / 25ns / 25 = 4
   */
  I2C1->CCR |= (4 << 0);

  /* 
     I2C TRISE
     MAX Trise for Fm is 300ns
     @40MHz Tplck = 25ns
     MAX Trise/Tplck = TRISE
     300ns/25ns = 12 (+1)
  */
  I2C1->TRISE |= (12 << 0);
  
  /* enable the periferal */
  I2C1->CR1 |= I2C_CR1_PE;
}

static RetStatus qc_flag_wait(I2C_TypeDef *I2Cx, int32 flag, BitStatus status, int32 time){

  

  return EXIT_OK;
}
