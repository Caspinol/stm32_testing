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

#define MAX_FLAG_WAIT 50000 /*eee 500ms wait ? */

#define WAIT(x)								\
	do{								\
		int32 t = kg_systick_get_tick();			\
		while((x)){						\
			if((kg_systick_get_tick() - t) > MAX_FLAG_WAIT){ \
				goto TIMEOUT;				\
			}						\
		}                                                       \
	}while(0)							\
		
#define I2C_START(I2CX) (I2CX->CR1 |= I2C_CR1_START)
#define I2C_STOP(I2CX) (I2CX->CR1 |= I2C_CR1_STOP)
#define I2C_RESTART(I2CX)                                       \
	do{							\
		I2C_STOP(I2CX);					\
		I2C_START(I2CX);				\
	}while(0)						\
		
#define I2C_CLEAR_ADDR(I2CX)                                    \
	do{							\
		int32 tmp;					\
		tmp = I2CX->SR1;				\
		tmp = I2CX->SR2;				\
		(void)(tmp);					\
	}while(0)						\
		
#define I2C_CHECK_SB(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_SB) == (I2C_SR1_SB & 0x0000ffffff)) == STATE)
#define I2C_CHECK_ADDR(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_ADDR) == (I2C_SR1_ADDR & 0x0000ffffff)) == STATE)
#define I2C_CHECK_OVR(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_OVR) == (I2C_SR1_OVR & 0x0000ffffff)) == STATE)
#define I2C_CHECK_AF(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_AF) == (I2C_SR1_AF & 0x0000ffffff)) == STATE)
#define I2C_CHECK_BERR(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_BERR) == (I2C_SR1_BERR & 0x0000ffffff)) == STATE)
#define I2C_CHECK_ARLO(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_ARLO) == (I2C_SR1_ARLO & 0x0000ffffff)) == STATE)
#define I2C_CHECK_TxE(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_TXE) == (I2C_SR1_TXE & 0x0000ffffff)) == STATE)
#define I2C_CHECK_RxNE(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_RXNE) == (I2C_SR2_BUSY & 0x0000ffffff)) == STATE)
#define I2C_CHECK_BTF(I2CX, STATE) (((I2CX->SR1 & I2C_SR1_BTF) == (I2C_SR2_BUSY & 0x0000ffffff)) == STATE)
#define I2C_CHECK_BUSY(I2CX, STATE) (((I2CX->SR2 & I2C_SR2_BUSY) == (I2C_SR2_BUSY & 0x0000ffffff)) == STATE)

#define I2C_SET_ACK(I2CX) (I2CX->CR1 |= I2C_CR1_ACK)
#define I2C_CLEAR_ACK(I2CX) (I2CX->CR1 &= (~I2C_CR1_ACK))

#define I2C_SET_READ(ADDR) (ADDR |= 0x01) 
#define I2C_SET_WRITE(ADDR) (ADDR &= 0x01)

static ret_status i2c_mem_write(I2C_TypeDef *I2Cx, int8 dev, int8 addr);
static ret_status i2c_mem_read(I2C_TypeDef *I2Cx, int8 dev, int8 addr);

void qc_i2c_init(){

	/* Init GPIOB clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	/* enable clock for i2c1 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	/* set pins as AF mode */
	GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER9_1);
	/* configure the GPIOB pin 6 adn 9 as AF4 */
	GPIOB->AFR[0] |= 0x04000000;
	GPIOB->AFR[1] |= 0x00000040;
	/* open drain for the i2c pins*/
	GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_9);
	/* both pins pull-up */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR9);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR9_0);
	/* High speed */
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR9);
			
	/* Enable I2C1 reset state */
	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	I2C1->CR1 |= I2C_CR1_SWRST;
	kg_delay(1000);
	/* Release I2C1 from reset state */
	I2C1->CR1 &= ~I2C_CR1_SWRST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
	
	/* disable the periferal */
	I2C1->CR1 &= ~I2C_CR1_PE;
	/* clock strech enabled */
	I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;
	/* clock is 40MHz */
	I2C1->CR2 |= (40 << 0);
	/* 7bit addressing no need to set own
	   address cause will be master */
	I2C1->OAR1 |= ((~I2C_OAR1_ADDMODE) | 43);
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
	I2C1->TRISE |= (13 << 0);
	
	/* enable the periferal */
	I2C1->CR1 |= I2C_CR1_PE;
}

ret_status qc_i2c_write(I2C_TypeDef *I2Cx, 
			int8 dev, /* device address */ 
			int8 addr, /* register address */
			int8 *data, /* pointer to data buffer */
			int8 size){ /* data buffer len */

	if(I2C_CHECK_BUSY(I2Cx, SET)){
		return EXIT_BUSY;
	}

	if(i2c_mem_write(I2Cx, dev, addr) != EXIT_OK){
		return EXIT_TIMEOUT;
	}

	while(size > 0){
		/* wait for TX flag */
		WAIT(I2C_CHECK_TxE(I2Cx, UNSET));

		/* write data */
		I2Cx->DR = *data++;
		size--;

		if(I2C_CHECK_BTF(I2Cx, SET) && size != 0){
			/* if more to send and Byte Transfer Finished 
			   write it now
			*/
			I2Cx->DR = *data++;
			size--;
		}
	}
	
	/* wait for transfer finish */
	WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
	/* and generate the STOP signal */
	I2C_STOP(I2Cx);
	return EXIT_OK;

 TIMEOUT:
	return EXIT_TIMEOUT;
}

ret_status qc_i2c_read(I2C_TypeDef *I2Cx, 
		       int8 dev, /* device address */ 
		       int8 addr, /* register address */
		       int8 *data, /* pointer to data buffer */
		       int8 size){ /* data buffer len */
	
	if(I2C_CHECK_BUSY(I2Cx, SET)){
		return EXIT_BUSY;
	}

	if(i2c_mem_read(I2Cx, dev, addr) != EXIT_OK){
		return EXIT_TIMEOUT;
	}

	if(size == 1){
		/* disable ACK */
		I2C_CLEAR_ACK(I2Cx);
		
		I2C_CLEAR_ADDR(I2Cx);
		/* generate STOP */
		I2C_STOP(I2Cx);

	}else if(size == 2){
		/* disable ACK */
		I2C_CLEAR_ACK(I2Cx);

		/* enable POS */
		I2Cx->CR1 |= I2C_CR1_POS;

		I2C_CLEAR_ADDR(I2Cx);
	}else{
		
		I2C_CLEAR_ADDR(I2Cx);
	}

	while(size > 0){
		if(size <= 3){
			if(size == 1){
				/* wait for whatever was need to be received*/
				WAIT(I2C_CHECK_RxNE(I2Cx, UNSET));

				(*data++) = I2Cx->DR;
				size--;
			}else if(size == 2){
				if(I2C_CHECK_BTF(I2Cx, UNSET)){
					return EXIT_FAIL;
				}
				
				I2C_STOP(I2Cx);

				(*data++) = I2Cx->DR;
				size--;
			}else{ /* so just read the last 3 bytes */
				if(I2C_CHECK_BTF(I2Cx, UNSET)){
					return EXIT_FAIL;
				}
				I2C_CLEAR_ACK(I2Cx);

				/* read the data */
				(*data++) = I2Cx->DR;
				size--;
				if(I2C_CHECK_BTF(I2Cx, UNSET)){
					return EXIT_FAIL;
				}

				I2C_STOP(I2Cx);

				/* read the data */
				(*data++) = I2Cx->DR;
				size--;
				
				/* read the data */
				(*data++) = I2Cx->DR;
				size--;
			}
		}else{
			WAIT(I2C_CHECK_RxNE(I2Cx, UNSET));
			/* just keep reading the data */
			(*data++) = I2Cx->DR;
			size--;
			
			if(I2C_CHECK_BTF(I2Cx, SET)){
				(*data++) = I2Cx->DR;
				size--;
			}
		}
	}
	/* disable POS */
	I2Cx->CR1 &= ~I2C_CR1_POS;
	WAIT(I2C_CHECK_BUSY(I2Cx, SET));
	      
	return EXIT_OK;

 TIMEOUT:

	return EXIT_TIMEOUT;
}

static ret_status i2c_mem_write(I2C_TypeDef *I2Cx, int8 dev, int8 addr){

	I2C_START(I2Cx);
	
	/* wait for START to send */
	WAIT(I2C_CHECK_SB(I2Cx, UNSET));
	
	/* write the device address
	   in write mode
	*/
	I2Cx->DR =  I2C_SET_WRITE(addr);
	
	/* and wait for address flag to set */
	WAIT(I2C_CHECK_ADDR(I2Cx, UNSET));
	
	I2C_CLEAR_ADDR(I2Cx);
	
	/* wait for TX flag */
	WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
	
	/* send the register address */
	I2Cx->DR = (addr & 0xff);

	return EXIT_OK;

 TIMEOUT:

	return EXIT_TIMEOUT;
}

static ret_status i2c_mem_read(I2C_TypeDef *I2Cx, int8 dev, int8 addr){
	
	I2C_SET_ACK(I2Cx);
	I2C_START(I2Cx);
	
	/* wait for START to send */
	WAIT(I2C_CHECK_SB(I2Cx, 0));
	
	/* write the device address
	   in write mode
	*/
	I2Cx->DR =  I2C_SET_WRITE(addr);
	
	/* and wait for address flag to set */
	WAIT(I2C_CHECK_ADDR(I2Cx, UNSET));
	
	I2C_CLEAR_ADDR(I2Cx);
	
	/* wait for TX flag */
	WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
	
	/* send the register address */
	I2Cx->DR = (addr & 0xff);
	
	/* wait for TX flag */
	WAIT(I2C_CHECK_TxE(I2Cx, UNSET));
	
	/* generate restart */
	I2C_RESTART(I2Cx);

	/* wait for START to send */
	WAIT(I2C_CHECK_SB(I2Cx, UNSET));
	
	/* now send device address again 
	   but to set it in read mode now
	*/
	I2Cx->DR =  I2C_SET_WRITE(addr);
	/* and wait for address flag to set */
	WAIT(I2C_CHECK_ADDR(I2Cx, UNSET));
	
	return EXIT_OK;

 TIMEOUT:

	return EXIT_TIMEOUT;
}
