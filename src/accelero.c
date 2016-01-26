#include <stm32f4xx.h>

#include "accelero.h"
#include "i2c.h"
#include "utils.h"

/* Device addresses */
#define ACC_SLAVE 0b00110010 
#define MAG_SLAVE 0b00111100

/* Accelerometer config registers */
#define ACC_CTRL_REG1 0x20
#define ACC_CTRL_REG4 0x23

/* Accelerometer output registers */
#define ACC_X_L 0x28
#define ACC_X_H 0x29
#define ACC_Y_L 0x2a
#define ACC_Y_H 0x2b
#define ACC_Z_L 0x2c
#define ACC_Z_H 0x2d

/* Magnetometer config */
#define MAG_CRA_REG_M 0x00
#define MAG_MR_REG_M 0x02

/* Magnetometer output registers */
#define MAG_X_L 0x03
#define MAG_X_H 0x04
#define MAG_Z_L 0x05
#define MAG_Z_H 0x06
#define MAG_Y_L 0x07
#define MAG_Y_H 0x08


static uint8_t out = 0;
static uint8_t i2c_initialized = 0; /* Flag indicating wheater i2 module is initialized */
static uint8_t temperature_sensor_en = 0; /* Is temp sensor enabled */

static void i2c_init(void){

	i2c_init_i2c();
	i2c_initialized = 1;
}

RETURN_STATUS acc_init_acc(void){
	/* 400Hz and x,y,z axis enabled */
	uint8_t conf_cr1 = 0x77;
	uint8_t check = 0;
	
	if(i2c_initialized == 0){
		i2c_init();
	}
	
	DEBUG("Init Accelero");
	
	i2c_write_data(ACC_SLAVE, ACC_CTRL_REG1, &conf_cr1, 1);
	i2c_read_data(ACC_SLAVE, ACC_CTRL_REG1, &check, 1);
	
	if(check != conf_cr1){
		DEBUG("Accelerometer initialization failed!");
		goto ERROR;
	}
	
	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

int16_t acc_get_acc_x(void){
	uint16_t out_x = 0;
	
	i2c_read_data(ACC_SLAVE, ACC_X_H, &out, 1);
	out_x = out << 8;
	i2c_read_data(ACC_SLAVE, ACC_X_L, &out, 1);
	out_x |= out;

	return out_x;
}

int16_t acc_get_acc_y(void){
	uint16_t out_y = 0;
	
	i2c_read_data(ACC_SLAVE, ACC_Y_H, &out, 1);
	out_y = out << 8;
	i2c_read_data(ACC_SLAVE, ACC_Y_L, &out, 1);
	out_y |= out;

	return out_y;
}

int16_t acc_get_acc_z(void){
	uint16_t out_z = 0;

	i2c_read_data(ACC_SLAVE, ACC_Z_H, &out, 1);
	out_z = out << 8;
	i2c_read_data(ACC_SLAVE, ACC_Z_L, &out, 1);
	out_z |= out;
	
	return out_z;
}

/********** MAGNETOMETER **********/
RETURN_STATUS acc_init_mag(uint8_t temp_sensor){
	/* Temperature sensor disabled, 30Hz */
	uint8_t cra_reg_m = 0b00010100;
	uint8_t mr_reg_m = 0;
	uint8_t check = 0;
	
	if(i2c_initialized == 0){
		i2c_init();
	}

	if(temp_sensor){
		/* Also initialize temperature sensor */
		cra_reg_m |= 0b10000000;
		temperature_sensor_en = 1;
	}
	
	DEBUG("Initializing Magnetometer");
	
	i2c_write_data(MAG_SLAVE, MAG_CRA_REG_M, &cra_reg_m, 1);
	i2c_read_data(MAG_SLAVE, MAG_CRA_REG_M, &check, 1);
	
	if(check != cra_reg_m){
		DEBUG("Magnetometer initialization failed!");
		goto ERROR;
	}

	i2c_write_data(MAG_SLAVE, MAG_CRA_REG_M, &mr_reg_m, 1);
	i2c_read_data(MAG_SLAVE, MAG_CRA_REG_M, &check, 1);
	if(check != mr_reg_m){
		DEBUG("Magnetometer initialization failed!");
		goto ERROR;
	}

	return EXIT_OK;

 ERROR:
	return EXIT_FAIL;
}

int16_t acc_get_mag_x(void){
	int16_t out_x = 0;
	
	i2c_read_data(MAG_SLAVE, MAG_X_H, &out, 1);
	out_x = out << 8;
	i2c_read_data(MAG_SLAVE, MAG_X_L, &out, 1);
	out_x |= out;

	return out_x;
}

int16_t acc_get_mag_y(void){
	int16_t out_y = 0;
	
	i2c_read_data(MAG_SLAVE, MAG_Y_H, &out, 1);
	out_y = out << 8;
	i2c_read_data(MAG_SLAVE, MAG_Y_L, &out, 1);
	out_y |= out;

	return out_y;
}

int16_t acc_get_mag_z(void){
	int16_t out_z = 0;

	i2c_read_data(MAG_SLAVE, MAG_Z_H, &out, 1);
	out_z = out << 8;
	i2c_read_data(MAG_SLAVE, MAG_Z_L, &out, 1);
	out_z |= out;
	
	return out_z;
}