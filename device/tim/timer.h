#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"

/* Global Types ------------------------------------------------------------ */

typedef enum {
    timObj_timer = 0,
    timObj_pwmOut,
    timObj_encoder,
} timObj_t;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void timer_init(u8 mode, u8 timNum, u16 chNum, u16 arr, u16 psc, u8 subPriority);

// 功能函数

void timer_pwm_set(u8 timNum, u8 chNum, u16 duty);
s16 timer_encoder_read(u8 timNum, sign_t sign);

#endif
