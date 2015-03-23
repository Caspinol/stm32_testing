#ifndef __I2C_H_
#define __I2C_H_

void kg_i2c_init();

ret_status qc_i2c_read(I2C_TypeDef *, int8, int8, int8*, int8);
ret_status qc_i2c_write(I2C_TypeDef *, int8, int8, int8 *, int8);

#endif
