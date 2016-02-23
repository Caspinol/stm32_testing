#ifndef __I2C_H__
#define __I2C_H__

void i2c_init_i2c(void);
RETURN_STATUS i2c_read_data(uint8_t slave, uint8_t const reg, uint8_t *buffer, uint8_t num);
RETURN_STATUS i2c_write_data(uint8_t slave, uint8_t const reg, uint8_t *data, uint8_t num);

#endif /* __I2C_H__ */
