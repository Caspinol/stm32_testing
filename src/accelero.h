#ifndef __ACCELERO_H__
#define __ACCELERO_H__

#include "utils.h"

enum {
	MX_AXIS = 0,
	MZ_AXIS,
	MY_AXIS	
} MAG_AXIS;

enum {
	AX_AXIS = 0,
	AY_AXIS,
	AZ_AXIS
} ACC_AXIS;

typedef struct {
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
}acc_xyz_t;

typedef struct {
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
}mag_xyz_t;

RETURN_STATUS acc_init_acc(void);
RETURN_STATUS acc_get_acc_xyz(acc_xyz_t * out_acc);
int16_t acc_get_acc_x(void);
int16_t acc_get_acc_y(void);
int16_t acc_get_acc_z(void);

RETURN_STATUS acc_init_mag(uint8_t temp_sensor);
RETURN_STATUS acc_get_mag_xyz(mag_xyz_t  * out_mag);
void acc_mag_calibrate(void);
float acc_get_heading(void);

#endif /* __ACCELERO_H__ */
