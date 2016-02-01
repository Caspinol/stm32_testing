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


RETURN_STATUS acc_init_acc(void);
int16_t acc_get_acc_x(void);
int16_t acc_get_acc_y(void);
int16_t acc_get_acc_z(void);

RETURN_STATUS acc_init_mag(uint8_t temp_sensor);
uint8_t acc_get_mag_xyz(int16_t * out_mag, int count);
void acc_mag_calibrate(void);
float acc_get_heading(void);

#endif /* __ACCELERO_H__ */
