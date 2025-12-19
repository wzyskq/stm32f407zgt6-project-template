#ifndef __WHEEL_H_
#define __WHEEL_H_

#include "main.h"

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void wheel_init(u8 whlNum);
void wheels_init(void);

// 功能函数

void wheel_stop(u8 whlNum);
void wheel_forward(u8 whlNum);
void wheel_backward(u8 whlNum);

void wheel_pwm_set(u8 whlNum, s16 speed);


#endif
