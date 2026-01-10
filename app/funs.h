#ifndef __FUNS_H
#define __FUNS_H

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

/* Exported Functions ------------------------------------------------------ */

void delay_us(u32 us);
void delay_ms(u32 ms);
void delay_s(u32 s);

u8 *strmatch_l(u8 *src, u8 *pattern);
u8 *strmatch_s(u8 *src, u8 *pattern);

float minf(float a, float b);
s16 abs16(s16 num);

#endif
