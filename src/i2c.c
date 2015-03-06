#include <stm32f401xc.h>
#include "systick.h"
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

#define MAX_FLAG_WAIT 500000 /*eee 500ms wait ? */

#define WAIT(x)                                             \
  do{                                                       \
    int32 t = kg_systick_get_tick();                        \
    while((x)){                                             \
      if((kg_systick_get_tick() - t) > MAX_FLAG_WAIT){      \
        return EXIT_TIMEOUT;                                \
      }                                                     \
    }                                                       \
  }while(0)                                                 \
    
#define I2C_START(I2CX) (I2CX->CR1 |= I2C_CR1_START)
#define I2C_STOP(I2CX) (I2CX->CR1 |= I2C_CR1_STOP)
#define I2C_RESTART(I2CX)                                       \
  do{                                                           \
    I2C_STOP(I2CX);                                             \
    I2C_START(I2CX);                                            \
  }while(0)                                                     \
    
#define I2C_CLEAR_ADDR(I2CX)                                    \
  do{                                                           \
    int32 tmp;                                                  \
    tmp = I2CX->SR1;                                            \
    tmp = I2CX->SR2;                                            \
    (void)(tmp);                                                \
  }while(0)                                                     \
    
#define I2C_CHECK_SB(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_SB) == STATE)
#define I2C_CHECK_ADDR(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_ADDR) == STATE)
#define I2C_CHECK_OVR(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_OVR) == STATE)
#define I2C_CHECK_AF(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_AF) == STATE)
#define I2C_CHECK_BERR(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_BERR) == STATE)
#define I2C_CHECK_ARLO(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_ARLO) == STATE)
#define I2C_CHECK_TxE(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_TXE) == STATE)
#define I2C_CHECK_RxNE(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_RXNE) == STATE)
#define I2C_CHECK_BTF(I2CX, STATE) ((I2CX->SR1 & I2C_SR1_BTF) == STATE)
#define I2C_CHECK_BUSY(I2CX, STATE) ((I2CX->SR2 & I2C_SR2_BUSY) == STATE)

#define I2C_SET_ACK(I2CX) (I2CX->CR1 |= I2C_CR1_ACK)
#define I2C_CLEAR_ACK(I2CX) (I2CX->CR1 &= (~I2C_CR1_ACK))

#define I2C_READ(ADDR) (ADDR |= 0x01) 
#define I2C_WRITE(ADDR) (ADDR &= 0x01)

static ret_status i2c_master_send(I2C_TypeDef *, int8);

void kg_i2c_init(){
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

  /* clock strrech enabled */
  I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;
  
  /* clock is 40MHz */
  I2C1->CR2 |= (40 << 0);

  /* 7bit addressing no need to set own
     address cause will be master */
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

ret_status kg_i2c_master_read(I2C_TypeDef *I2Cx, int8 address, int8 *data, int size){
  
  if(size < 1){
    return EXIT_FAIL;
  }

  /* maybe i should wait for a while for it to become ready ? */
  if(I2C_CHECK_BUSY(I2Cx, SET)){
    return EXIT_FAIL;
  }
  
  /* if address send failed just quit */
  if(i2c_master_send(I2Cx, I2C_READ(address)) != EXIT_OK){
    return EXIT_FAIL;
  }
  
  /* another weird stuff */
  if(size == 1){
    /* close down sequence for 1 byte read
       page 471
    */
    I2C_CLEAR_ACK(I2Cx);
    I2C_CLEAR_ADDR(I2Cx);
    I2C_STOP(I2Cx);
  }else if(size == 2){
    /* 2 byte reception sequence
     */
    while(I2C_CHECK_ADDR(I2Cx, UNSET));
    I2C_CLEAR_ACK(I2Cx);
    I2Cx->CR1 |= I2C_CR1_POS;
    I2C_CLEAR_ADDR(I2Cx);
  }else{
    I2C_SET_ACK(I2Cx);
    I2C_CLEAR_ADDR(I2Cx);
  }  

  /* now with this stuff out of way 
     data can be finally read
  */
  while(size > 0){
    if(size == 1){
      WAIT(I2C_CHECK_RxNE(I2Cx, UNSET));
      (*data++) = I2Cx->DR; /* get data */
      size--;
    }else if(size == 2){
      /* similar to the 3 bytes read*/
      WAIT(I2C_CHECK_BTF(I2Cx, UNSET));
      
      I2C_STOP(I2Cx);
      
      /* read remaining 2 bytes*/
      (*data++) = I2Cx->DR;
      size--;
      (*data++) = I2Cx->DR;
      size--;
    }else if(size == 3){
      /* special sequence for last 3 bytes */
      WAIT(I2C_CHECK_BTF(I2Cx, UNSET));
      I2C_CLEAR_ACK(I2Cx);
      
      (*data++) = I2Cx->DR;
      size--;

      WAIT(I2C_CHECK_BTF(I2Cx, UNSET));

      I2C_STOP(I2Cx);

      /* read remaining 2 bytes*/
      (*data++) = I2Cx->DR;
      size--;
      (*data++) = I2Cx->DR;
      size--;

    }else{
      WAIT(I2C_CHECK_RxNE(I2Cx, UNSET));
      (*data++) = I2Cx->DR; /* get data */
      size--;
      if(I2C_CHECK_BTF(I2Cx, SET)){
        (*data++) = I2Cx->DR; /* get data */
        size--;
      }
    }
  }
  
  /* disable POS */
  I2Cx->CR1 &= ~I2C_CR1_POS;
  
  /*wait till module becomes UN-BUSY */
  WAIT(I2C_CHECK_BUSY(I2Cx, SET));
  return EXIT_OK;
}

ret_status kg_i2c_master_write(I2C_TypeDef *I2Cx, int8 address, int8 *data, int size){
  
  /* maybe i should wait for a while for it to become ready ? */
  if(I2C_CHECK_BUSY(I2Cx, SET)){
    return EXIT_FAIL;
  }

  /* if address send failed just quit */
  if(i2c_master_send(I2Cx, I2C_WRITE(address)) != EXIT_OK){
    return EXIT_FAIL;
  }

  /* clear ADDR flag */
  I2C_CLEAR_ADDR(I2Cx);

  while(size > 0){
    /* wait for DR to become empty*/
    WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
    
    I2Cx->DR = (*data++);
    size--;
    
    /*dont fully understand this mechanism but...
      if BTF is set it can be only cleared by writing to DR
    */
    if(I2C_CHECK_BTF(I2Cx, SET) && size > 0){
      I2Cx->DR = (*data++);
      size--;
    }
  }

  /* wait till transmission is finished*/
  WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
  /* and then send STOP to slave */
  I2C_STOP(I2Cx);

  /*wait till module becomes UN-BUSY */
  WAIT(I2C_CHECK_BUSY(I2Cx, SET));

  return EXIT_OK;
}

static ret_status i2c_master_send(I2C_TypeDef *I2Cx, int8 address){

  I2C_START(I2Cx);

  /* wait for START to send */
  WAIT(I2C_CHECK_SB(I2Cx, UNSET));

   /* write the address with LSB 0 to indicate write*/
  I2Cx->DR = address;
  
  /* and wait for address flag to set */
  WAIT(I2C_CHECK_ADDR(I2Cx, UNSET));
  
  /* also check for Acknowledge failure */
  if(I2C_CHECK_AF(I2Cx, SET)){
    return EXIT_FAIL;
  }
  return EXIT_OK;
}
