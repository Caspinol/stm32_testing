#ifndef __I2C_H_
#define __I2C_H_

void kg_i2c_init();
ret_status kg_i2c_master_read(I2C_TypeDef *, int8, int8 *, int);
ret_status kg_i2c_master_write(I2C_TypeDef *, int8, int8 *, int);

#endif
