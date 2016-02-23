#ifndef __PWM_H__
#define __PWM_H__

#define PWM_SET_DUTY(DUTY) (uint16_t)(((8399 + 1) * (DUTY)) / 100 - 1)

void pwm_init_pwm(void);
void pwm_set_compare1(uint16_t pwm_val);
void pwm_set_compare2(uint16_t pwm_val);
void pwm_set_compare3(uint16_t pwm_val);
void pwm_set_compare4(uint16_t pwm_val);

#endif
