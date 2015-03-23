#ifndef __UTILS_H_
#define __UTILS_H_

#define FALSE 0
#define TRUE !FALSE

#define SET_s(TIME) ((TIME) * 1000000)
#define SET_ms(TIME) ((TIME) * 1000)

typedef uint32_t int32;
typedef uint16_t int16;
typedef uint8_t int8;

typedef enum{
  UNSET = 0,
  SET
}bit_status;

typedef enum{
  EXIT_OK = 0,
  EXIT_FAIL,
  EXIT_TIMEOUT,
  EXIT_BUSY
}ret_status;

void kg_delay(int32);

#endif
