#ifndef __TIME_H__
#define __TIME_H__


#define MAX_FLAG_WAIT 500
#define WAIT(X)								\
	do{								\
		uint_fast32_t t = get_systick();			\
		while((X)){						\
			if((get_systick() - t) > MAX_FLAG_WAIT){	\
				goto ERROR;				\
			}						\
		}                                                       \
	}while(0)


void Delay(uint_fast32_t const time);
void update_systick(void);
uint_fast32_t get_systick(void);
void tim_init_tim3(void);

#endif /* __TIME_H__ */
