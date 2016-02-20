#ifndef __GYRO_H__
#define __GYRO_H__

#include "utils.h"

#define SENSITIVITY 0.00875

/* Stores the raw values (pre-processed) */
typedef struct {
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
}gyro_xyz_raw_t;

/* Processed usable values */
typedef struct {
	float x;
	float y;
	float z;
}gyro_xyz_t;

RETURN_STATUS gyro_init_gyro(void);
RETURN_STATUS gyro_get_xyz(gyro_xyz_t * g_xyz);


#endif /* __GYRO_H__ */
