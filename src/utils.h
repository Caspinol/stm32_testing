#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

#define FALSE  0
#define TRUE   !FALSE
#define SET    TRUE

#ifdef SH_DEBUG
#define DEBUG(M, ...)     printf("DEBUG: %s:%d " M "\n",__FILE__, __LINE__, ##__VA_ARGS__)
#else
 #define DEBUG(M, ...)
#endif


typedef enum{
  EXIT_OK = 0,
  EXIT_FAIL,
  EXIT_TIMEOUT,
  EXIT_BUSY
} RETURN_STATUS;


#endif
