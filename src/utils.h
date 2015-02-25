#ifndef __UTILS_H_
#define __UTILS_H_

#define FALSE 0
#define TRUE !FALSE

#define SET_s(TIME) ((TIME) * 1000000)
#define SET_ms(TIME) ((TIME) * 1000)

typedef uint32_t int32;
typedef uint16_t int16;
typedef uint8_t ubyte;

enum{
  UNSET = 0,
  SET
}BitStatus;

enum{
  EXIT_OK = 0,
  EXIT_FAIL
}RetStatus;

void qc_delay(int32);

#endif
