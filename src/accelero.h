#ifndef __ACCELERO_H__
#define __ACCELERO_H__

#include "utils.h"

RETURN_STATUS acc_init_acc(void);
int16_t acc_get_acc_x(void);
int16_t acc_get_acc_y(void);
int16_t acc_get_acc_z(void);

RETURN_STATUS acc_init_mag(uint8_t temp_sensor);
int16_t acc_get_mag_x(void);
int16_t acc_get_mag_y(void);
int16_t acc_get_mag_z(void);

#endif /* __ACCELERO_H__ */
