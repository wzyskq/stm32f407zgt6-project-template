#ifndef __TIMER_H
#define __TIMER_H

#include "main.h"

/* Global Functions -------------------------------------------------------- */

void timer_init(u8 timNum, u16 arr, u16 psc, u8 subPriority);
void timer_pwm_init(u8 timNum, u16 chNum, u16 arr, u16 psc, u8 subPriority);
void timer_pwm_set(u8 timNum, u8 chNum, u16 duty);

// void timer_init_2(void);
// void Tim_4_Init(void);
// void Encoder_Init(void);

#endif
