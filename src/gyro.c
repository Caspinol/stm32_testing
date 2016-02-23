#include <stm32f4xx.h>

#include "gyro.h"
#include "time.h"
#include "spi.h"
#include "utils.h"

#define WHO_AM_I 0x0f
#define I_AM_GROOT 0b11010100
#define GYRO_CTRL_REG1 0x20
#define GYRO_CTRL_REG4 0x23

#define GYRO_OUT_X_L 0x28
#define GYRO_OUT_X_H 0x29
#define GYRO_OUT_Y_L 0x2a
#define GYRO_OUT_Y_H 0x2b
#define GYRO_OUT_Z_L 0x2c
#define GYRO_OUT_Z_H 0x2d

#define CALIB_SAMPLES 100

static uint8_t gyro_initialized = 0;
static float g_xoffset = 0;
static float g_yoffset = 0;
static float g_zoffset = 0;

/* Static functions */
static RETURN_STATUS gyro_calibrate(uint8_t const samples);
static RETURN_STATUS gyro_get_xyz_raw(gyro_xyz_raw_t * g_raw);

RETURN_STATUS gyro_init_gyro(void){

	spi_init_spi();

	/* DR = 00, BW = 00, PD = 1, Z, X and Y enabled */
	uint8_t setup = 0b00001111;
	if(spi_write_data(GYRO_CTRL_REG1, setup)){
		goto ERROR;
	}
	
	uint8_t tmp = 0;
	spi_read_data(GYRO_CTRL_REG1, &tmp);
	if(setup != tmp){
		DEBUG("GYRO initialization failed. Expected [%d] but got [%d].", setup, tmp);
		goto ERROR;
	}

	setup = 0b00000000;
	if(spi_write_data(GYRO_CTRL_REG4, setup)){
		goto ERROR;
	}
	
	uint8_t who_am_i = 0;
	spi_read_data(WHO_AM_I, &who_am_i);
	if(who_am_i != I_AM_GROOT){
		DEBUG("He is not GROOT");
		goto ERROR;
	}

	gyro_initialized = 1;

	gyro_calibrate(CALIB_SAMPLES);
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

static RETURN_STATUS gyro_calibrate(uint8_t const samples){

	gyro_xyz_raw_t g_raw;
	float g_x = 0,
		g_y = 0,
		g_z = 0;
	
	if(!samples) goto ERROR;
	
	int i = samples;
	do{
 		if(gyro_get_xyz_raw(&g_raw)) goto ERROR;
		g_x += g_raw.x;
		g_y += g_raw.y;
		g_z += g_raw.z;
 	}while(--i);
	
 	g_xoffset = g_x / samples;
	g_yoffset = g_y / samples;
	g_zoffset = g_z / samples;
	
	DEBUG("Gyro offset: X = [%g], Y = [%g], Z = [%g]",
	      g_xoffset, g_yoffset, g_zoffset);
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

static RETURN_STATUS gyro_get_xyz_raw(gyro_xyz_raw_t * g_raw){
	uint8_t x_l, x_h, y_l, y_h, z_l, z_h;
	
	if(!gyro_initialized) goto ERROR;

	spi_read_data(GYRO_OUT_X_L, &x_l);
	spi_read_data(GYRO_OUT_X_H, &x_h);

	g_raw->x = ((x_h << 8) | x_l);

	spi_read_data(GYRO_OUT_Y_L, &y_l);
	spi_read_data(GYRO_OUT_Y_H, &y_h);

	g_raw->y = ((y_h << 8) | y_l);

	spi_read_data(GYRO_OUT_Z_L, &z_l);
	spi_read_data(GYRO_OUT_Z_H, &z_h);

	g_raw->z = ((z_h << 8) | z_l);

	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

RETURN_STATUS gyro_get_xyz(gyro_xyz_t * g_xyz){
	
	gyro_xyz_raw_t g_raw;

	if(gyro_get_xyz_raw(&g_raw)) goto ERROR; 

	g_xyz->x = (float) SENSITIVITY * (g_raw.x - g_xoffset);
	g_xyz->y = (float) SENSITIVITY * (g_raw.y - g_yoffset);
	g_xyz->z = (float) SENSITIVITY * (g_raw.z - g_zoffset);
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}
