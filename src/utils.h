#ifndef __UTILS_H__
#define __UTILS_H__

#define FALSE  0
#define UNSET  FALSE
#define TRUE   !FALSE
#define SET    TRUE

typedef uint32_t int32;
typedef uint16_t int16;
typedef uint8_t int8;

typedef enum{
  EXIT_OK = 0,
  EXIT_FAIL,
  EXIT_TIMEOUT,
  EXIT_BUSY
}ret_status;

void Delay(__IO int32 nTime);

void TimingDelay_Decrement(void);

#endif
