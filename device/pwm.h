#ifndef __PWM_H_
#define __PWM_H_

#include "main.h"

void pwm_init(u16 arr, u16 psc);
void pwm_set_rt(s16 compare);
void pwm_set_lt(s16 compare);

#endif
