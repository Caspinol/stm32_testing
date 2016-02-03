#ifndef __GYRO_H__
#define __GYRO_H__

#include "utils.h"

RETURN_STATUS gyro_init_gyro(void);
int16_t gyro_get_X(void);
int16_t gyro_get_Y(void);

#endif /* __GYRO_H__ */
