#ifndef __ACCELERO_H__
#define __ACCELERO_H__

#include "utils.h"

typedef struct {
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
}acc_xyz_t;

typedef struct {
	float pitch;
	float roll;
}acc_angle_t;

typedef struct {
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
}mag_xyz_t;

/* Accelerometer functions */
RETURN_STATUS acc_init_acc(void);
RETURN_STATUS acc_get_acc_xyz(acc_xyz_t * out_acc);
RETURN_STATUS acc_get_acc_angle(acc_angle_t * acc_a_out);

/* Magnetometer functions */
RETURN_STATUS acc_init_mag(uint8_t temp_sensor);
RETURN_STATUS acc_get_mag_xyz(mag_xyz_t  * out_mag);
void acc_mag_calibrate(void);

float acc_get_heading(void);

#endif /* __ACCELERO_H__ */
