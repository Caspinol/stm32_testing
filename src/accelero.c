#include <stm32f4xx.h>
#include <math.h>

#include "time.h"
#include "utils.h"
#include "accelero.h"
#include "i2c.h"

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

#define CALIBRATION_DELAY 200 // ms
#define CALIBRATION_DURATION (5000 / CALIBRATION_DELAY)

#define M_PI 3.14159
#define D180_OVER_PI (float)(180.0 / M_PI)
#define PI_OVER_D180 (float)(M_PI / 180.0)

#define DEG_TO_RAD(DEG) (float)((DEG) * PI_OVER_D180)
#define RAD_TO_DEG(RAD) (float)((RAD) * D180_OVER_PI)

static uint8_t calibration_done = 0;
static uint8_t i2c_initialized = 0; /* Flag indicating wheater i2 module is initialized */
static uint8_t temperature_sensor_en = 0; /* Is temp sensor enabled */

/* Used for calibration */
static int16_t mag_x_max = 384,
	mag_x_min = -342,
	mag_y_max = 477,
	mag_y_min = -471,
	mag_z_max = 506,
	mag_z_min = -476;


static void get_pitch_roll(float * pitch, float * roll);
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

RETURN_STATUS acc_get_acc_xyz(acc_xyz_t * acc_out){
	uint8_t out_h = 0;
	uint8_t out_l = 0;
	
	i2c_read_data(ACC_SLAVE, ACC_X_H, &out_h, 1);
	i2c_read_data(ACC_SLAVE, ACC_X_L, &out_l, 1);
	acc_out->x = ((out_h << 8) + out_l) >> 4;

	i2c_read_data(ACC_SLAVE, ACC_Y_H, &out_h, 1);
	i2c_read_data(ACC_SLAVE, ACC_Y_L, &out_l, 1);
	acc_out->y = ((out_h << 8) + out_l) >> 4;

	i2c_read_data(ACC_SLAVE, ACC_Z_H, &out_h, 1);
	i2c_read_data(ACC_SLAVE, ACC_Z_L, &out_l, 1);
	acc_out->z = ((out_h << 8) + out_l) >> 4;
	
	return EXIT_OK;
}

RETURN_STATUS acc_get_acc_angle(acc_angle_t * acc_angle){
	float pitch, roll;

	get_pitch_roll(&pitch, &roll);

	acc_angle->pitch = pitch;
	acc_angle->roll = roll;
	
	return EXIT_OK;
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
	
	i2c_write_data(MAG_SLAVE, MAG_CRA_REG_M, &cra_reg_m, 1);
	i2c_read_data(MAG_SLAVE, MAG_CRA_REG_M, &check, 1);
	
	if(check != cra_reg_m){
		DEBUG("Magnetometer initialization failed!");
		goto ERROR;
	}

	i2c_write_data(MAG_SLAVE, MAG_MR_REG_M, &mr_reg_m, 1);
	i2c_read_data(MAG_SLAVE, MAG_MR_REG_M, &check, 1);
	if(check != mr_reg_m){
		DEBUG("Magnetometer initialization failed!");
		goto ERROR;
	}

	return EXIT_OK;
 ERROR:
	return EXIT_FAIL;
}

RETURN_STATUS acc_get_mag_xyz(mag_xyz_t * out_mag){
	uint8_t out[6] = {0};
	
	i2c_read_data(MAG_SLAVE, MAG_X_L, out, 6);
	out_mag->x = (int16_t) (out[0] << 8) + out[1];
	out_mag->z = (int16_t) (out[2] << 8) + out[3];
	out_mag->y = (int16_t) (out[4] << 8) + out[5];

	return EXIT_OK;
}

void acc_mag_calibrate(void){

	mag_xyz_t m_out;
	if(!calibration_done){
		int i = CALIBRATION_DURATION;
		do{
			acc_get_mag_xyz(&m_out);
			
			if(m_out.x < mag_x_min) mag_x_min = m_out.x;
			if(m_out.x > mag_x_max) mag_x_max = m_out.x;
			
			if(m_out.z < mag_z_min) mag_z_min = m_out.z;
			if(m_out.z > mag_z_max) mag_z_max = m_out.z;
			
			if(m_out.y < mag_y_min) mag_y_min = m_out.y;
			if(m_out.y > mag_y_max) mag_y_max = m_out.y;
			
			Delay(CALIBRATION_DELAY);
		}while(--i);
		
		calibration_done = 1;
	}
}

float acc_get_heading(void){

	mag_xyz_t m_out;
	float pitch, roll;

	get_pitch_roll(&pitch, &roll);
	
	float c_pitch = cos(pitch);
	float s_pitch = sin(pitch);
	float s_roll = sin(roll);
	float c_roll = cos(roll);
	
	
	acc_get_mag_xyz(&m_out);
	
	/* 
	   Calculate the offset based on the min max values
	   Equation 12
	*/
	float MAG_Xc = (float)(m_out.x - mag_x_min) / (mag_x_max - mag_x_min) * 2 - 1;
	float MAG_Yc = (float)(m_out.y - mag_y_min) / (mag_y_max - mag_y_min) * 2 - 1;
	float MAG_Zc = (float)(m_out.z - mag_z_min) / (mag_z_max - mag_z_min) * 2 - 1;
	
	float MAG_Xh = MAG_Xc * c_pitch + MAG_Zc * s_pitch;
	float MAG_Yh = MAG_Xc * s_roll * s_pitch + MAG_Yc * c_roll - MAG_Zc * s_roll * c_pitch;
	
	/* Now we can find out the heading */
	float heading = RAD_TO_DEG(atan2(MAG_Yh, MAG_Xh));
	
	if(heading < 0){
		heading += 360;
	}

	return heading;
}

static void get_pitch_roll(float * pitch, float * roll){

	float z_squared;
	acc_xyz_t acc_out;
	
	acc_get_acc_xyz(&acc_out);
        z_squared = acc_out.z * acc_out.z;
	
	*pitch = RAD_TO_DEG(atan2(acc_out.x, sqrt(acc_out.y * acc_out.y + z_squared)));
	*roll = RAD_TO_DEG(atan2(acc_out.y, sqrt(acc_out.x * acc_out.x + z_squared)));
}
