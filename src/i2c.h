#ifndef __I2C_H__
#define __I2C_H__

void i2c_init_i2c(void);
uint8_t i2c_read_data(uint8_t reg, uint8_t *buffer, uint8_t num);
uint8_t i2c_write_data(uint8_t reg, uint8_t *data, uint8_t num);

#endif /* __I2C_H__ */
