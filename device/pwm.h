#ifndef __PWM_H_
#define __PWM_H_

void pwm_init(WORD arr, WORD psc);
void pwm_set_rt(WORD compare);
void pwm_set_lt(WORD compare);

#endif
