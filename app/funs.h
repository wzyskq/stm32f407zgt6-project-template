#ifndef __FUNS_H
#define __FUNS_H

#include "main.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 阻塞延时 */

void delay_us(u32 us); // 微秒级延时，范围：0..25,565,281
void delay_ms(u32 ms); // 毫秒级延时，范围：0..4,294,967,295
void delay_s(u32 s);   // 秒级延时，范围：0..4,294,967,295

#endif
