#ifndef __I2C_H_
#define __I2C_H_

#define QC_I2C1_STOP (I2C1->CR1 |= I2C_CR1_STOP)
#define QC_I2C1_START (I2C1->CR1 |= I2C_CR1_START)

void qc_i2c_init();
void qc_i2c_write();

#endif
