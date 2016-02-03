#ifndef __SPI_H__
#define __SPI_H__

void spi_init_spi(void);
RETURN_STATUS spi_write_data(uint8_t reg, uint8_t data);
RETURN_STATUS spi_read_data(uint8_t reg, uint8_t * buf);

#endif /* __SPI_H__ */
