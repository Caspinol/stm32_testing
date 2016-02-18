#ifndef __GYRO_H__
#define __GYRO_H__

#include "utils.h"

#define SENSITIVITY 0.00875

/* Stores the raw values (pre-processed) */
struct gyro_xyz_raw_t {
	int16_t x;
	int16_t y;
	int16_t z;
};

/* Processed usable values */
struct gyro_xyz_t {
	float x;
	float y;
	float z;
};

RETURN_STATUS gyro_init_gyro(void);
RETURN_STATUS gyro_get_xyz(struct gyro_xyz_t * g_xyz);


#endif /* __GYRO_H__ */
