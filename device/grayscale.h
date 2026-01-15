#ifndef __GRAYSCALE_H_
#define __GRAYSCALE_H_

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

extern u8 graySrc[];

/* Exported Functions ------------------------------------------------------ */

// 初始化函数

void gray_init(u8 grayNum);
void grays_init(void);

// 功能函数

void gray_scan(void);

// 算法函数

u8 gray_filter_core(u8 *arr, u8 *res);
s8 gray_quant_core(u8 *src, u8 *res);

#endif
