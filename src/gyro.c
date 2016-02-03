#include <stm32f4xx.h>

#include "gyro.h"
#include "time.h"
#include "spi.h"

#define WHO_AM_I 0x0f
#define I_AM_GROOT 0b11010100
#define GYRO_CTRL_REG1 0x20

#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2a
#define GYRO_OUT_Y_H 0x2b
#define GYRO_OUT_Z_L 0x2c
#define GYRO_OUT_Z_H 0x2d

static uint8_t gyro_initialized = 0;

RETURN_STATUS gyro_init_gyro(void){
	spi_init_spi();
	/* DR = 11, BW = 00, PD = 1, X and Y enabled */
	uint8_t setup = 0b11001011;
	if(spi_write_data(GYRO_CTRL_REG1, setup)){
		goto ERROR;
	}
	uint8_t tmp = 0;
	spi_read_data(GYRO_CTRL_REG1, &tmp);
	if(setup != tmp){
		DEBUG("GYRO initialization failed. Expected [%d] but got [%d].", setup, tmp);
		goto ERROR;
	}
	
	uint8_t who_am_i = 0;
	spi_read_data(WHO_AM_I, &who_am_i);
	if(who_am_i != I_AM_GROOT){
		DEBUG("He is not GROOT");
		goto ERROR;
	}

	gyro_initialized = 1;
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

int16_t gyro_get_X(void){
	uint8_t x_l, x_h;
	
	if(!gyro_initialized) return -1;

	spi_read_data(GYRO_OUT_X_L, &x_l);
	spi_read_data(GYRO_OUT_X_H, &x_h);

	return (x_h << 8) + x_l;
}

int16_t gyro_get_Y(void){
	uint8_t y_l, y_h;
	
	if(!gyro_initialized) return -1;

	spi_read_data(GYRO_OUT_Y_L, &y_l);
	spi_read_data(GYRO_OUT_Y_H, &y_h);

	return (y_h << 8) + y_l;
}

