#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_i2c.h>

#include "utils.h"
#include "i2c.h"

#define I2C_SET_READ(ADDR) (uint8_t)((ADDR) |= 0x01) 
#define I2C_SET_WRITE(ADDR) (uint8_t)((ADDR) &= ~(1 << 0))

static void generate_start(void);

void i2c_init_i2c(void){

	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;

	/* Firs enable clock for I2C peripheral */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* and GPIOB */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* then configure pins */
	GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_9);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Now select AF function */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);

	/* 
	   The pins on Discovery board uset for interfacing are:
	   SCL -> PB6
	   SDA -> PB9
	*/
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed = 5000;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	/* We're Master so no address needed */
	I2C_InitStruct.I2C_OwnAddress1 = 0;
	I2C_Init(I2C1, &I2C_InitStruct);

	I2C_Cmd(I2C1, ENABLE);
}

/*
  Reads the data from the specified register address
  Reads from @reg
  Into @buffer
  As much as @num bytes
 */
uint8_t i2c_read_data(uint8_t slave, uint8_t reg, uint8_t *buffer, uint8_t num){

	if(!num) return 1;
	
	generate_start();
	
	/* Send device address in WRITE mode */
	I2C_SendData(I2C1, I2C_SET_WRITE(slave));

	/* Check for ADDR bit */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Send register address */
	I2C_SendData(I2C1, reg);

	/* Wait for it... */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Now RESTART the process */
	I2C_GenerateSTOP(I2C1, ENABLE);
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Wait for START bit to set */
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));

	/* Send device address but this time in READ mode*/
	I2C_SendData(I2C1, I2C_SET_READ(slave));

	/* Check for ADDR and clear */
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

	/* Now we should be ready to receive num bytes from slave */
	while(num){

		if(1 == num){
			/* NACK the data */
			I2C_AcknowledgeConfig(I2C1, DISABLE);
			I2C_GenerateSTOP(I2C1, ENABLE);
		}else{
			/* ACK the data */
			I2C_AcknowledgeConfig(I2C1, ENABLE);
		}

		/* Wait for the data */
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*buffer++ = I2C_ReceiveData(I2C1);
		num--;
	}
	
	return 0;
}

/*
  Writes specifued date into the register
  Writes @data
  Into @reg
  As much as @num bytes
*/
uint8_t i2c_write_data(uint8_t slave, uint8_t reg, uint8_t *data, uint8_t num){

	if(!num) return 1;

	generate_start();

	/* Are we MASTER yet? */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send WR address */
	I2C_SendData(I2C1, I2C_SET_WRITE(slave));

	/* Check if slave accepted it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, reg);

	/* Now continue sending rest of data */
	while(num--){
		while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));  
		I2C_SendData(I2C1, *data++);
	}

	/* Wait for previous transmission to finish */
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
	/* And... */
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	return 0;
}

static void generate_start(void){

	/* wait for peripheral to become available */
	/* TODO: need to add some timeout for all the waits */
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C1, ENABLE);
	
	/* Wait for START bit to set */
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));

}
