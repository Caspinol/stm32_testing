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
static float gyro_x_offset = 0;
static float gyro_y_offset = 0;
static float gyro_z_offset = 0;

/* Low Pass Filter storage variables */
#define LP_BETA 0.025

static float lp_x = 0,
	lp_y = 0,
	lp_z = 0;

/* Static functions */
static RETURN_STATUS gyro_calibrate(uint8_t samples);
static RETURN_STATUS gyro_get_xyz_raw(struct gyro_xyz_raw_t * g_raw);

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

	setup = 0b10000000;
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

static RETURN_STATUS gyro_calibrate(uint8_t samples){
	struct gyro_xyz_raw_t g_raw;
	float g_x = 0,
		g_y = 0,
		g_z = 0;

	if(!samples) goto ERROR;

	DEBUG("Calibrating Gyro");
	for(int i = 0; i < samples; i++){
 		if(gyro_get_xyz_raw(&g_raw)) goto ERROR;
		g_x += g_raw.x;
		g_y += g_raw.y;
		g_z += g_raw.z;
 	}
	
 	gyro_x_offset = g_x / samples;
	gyro_y_offset = g_y / samples;
	gyro_z_offset = g_z / samples;

	DEBUG("Gyro offset: X = [%g], Y = [%g], Z = [%g]",
	      gyro_x_offset, gyro_y_offset, gyro_z_offset);
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

static RETURN_STATUS gyro_get_xyz_raw(struct gyro_xyz_raw_t * g_raw){
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

/* Returns Gyro XYZ data after applying Low Pass Filter
   The LPF is just:
   Y(n) = (1-ß)*Y(n-1) + (ß*X(n))) = Y(n-1) - (ß*(Y(n-1)-X(n)));
*/
RETURN_STATUS gyro_get_xyz(struct gyro_xyz_t * g_xyz){
	
	struct gyro_xyz_raw_t g_raw;

	if(gyro_get_xyz_raw(&g_raw)) goto ERROR; 

	//g_raw.x = SENSITIVITY * (g_raw.x);
	//g_raw.y = SENSITIVITY * (g_raw.y);
	//g_raw.z = SENSITIVITY * (g_raw.z);
	
	//lp_x = lp_x - (LP_BETA * (lp_x - g_raw.x));
	//lp_y = lp_y - (LP_BETA * (lp_y - g_raw.y));
	//lp_z = lp_z - (LP_BETA * (lp_z - g_raw.z));

	g_xyz->x = (float) SENSITIVITY * g_raw.x;
	g_xyz->y = (float) SENSITIVITY * g_raw.y;
	g_xyz->z = (float) SENSITIVITY * g_raw.z;
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}
