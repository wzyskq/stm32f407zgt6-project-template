#ifndef __GRAYSCALE_H_
#define __GRAYSCALE_H_

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// 灰度传感器配置索引
typedef enum
{
    gray1 = 0,
    gray2,
    gray3,
    gray4,
    gray5,
    gray6,
    gray7,
    gray8,
    grayNum // 灰度传感器末尾索引，仅用于统计灰度传感器数量
} gray_e;

// 灰度传感器配置结构体
typedef struct
{
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin;
} gray_s;

/* Global Variables -------------------------------------------------------- */

extern u8 graySrc[];

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void gray_init(gray_e idx);
void grays_init(void);

// 功能函数

void gray_scan(void);

// 算法函数

u8 gray_filter_core(u8 *arr, u8 *res);
s8 gray_quant_core(u8 *src, u8 *res);

#endif
