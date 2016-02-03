#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_spi.h>

#include "time.h"
#include "utils.h"
#include "spi.h"

#define SPI_SET_READ(ADDR) (uint8_t)((ADDR) |= 0x80) 
#define SPI_SET_WRITE(ADDR) (uint8_t)((ADDR) &= ~(1 << 8))

#define SPI_CS_LOW GPIO_ResetBits(GPIOE, GPIO_Pin_3)
#define SPI_CS_HIGH GPIO_SetBits(GPIOE, GPIO_Pin_3)

void spi_init_spi(void){

	/* PA5 - SCKL, PA7 - MOSI, PA6 - MISO, PE3 - CS (SPI1)*/
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;

	/* First enable clock for SPI peripheral */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	/* and GPIOA */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);
	
	/* Now select AF function */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	/* then configure pins */
	GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* PE5 will be controlled manually */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	/* Now SPI */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;

	SPI_Init(SPI1, &SPI_InitStruct);

	SPI_Cmd(SPI1, ENABLE);
}

RETURN_STATUS spi_write_data(uint8_t reg, uint8_t data){

	SPI_CS_LOW; // Start by seleting chip

	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, reg);
	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); // dummy data, can ignore

	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, data);
	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); // dummy data, can ignore

	SPI_CS_HIGH; // Unselect chip
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

RETURN_STATUS spi_read_data(uint8_t reg, uint8_t *buf){

	SPI_CS_LOW;

	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, SPI_SET_READ(reg));
	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(SPI1); // dummy data, can ignore	

	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, 0); // keep generating clock
	WAIT(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));

	SPI_CS_HIGH;

	*buf = SPI_I2S_ReceiveData(SPI1);
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}
