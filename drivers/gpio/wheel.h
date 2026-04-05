#ifndef __WHEEL_H_
#define __WHEEL_H_

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// 轮子配置索引
typedef enum {
    wheelLt = 0,
    wheelRt,
    wheelNum // 轮子末尾索引，仅用于统计轮子数量
} wheel_e;

// 轮子配置结构体
typedef struct {
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin[2];
    tim_e tim;
    u8 ch;
} wheel_s;

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void wheel_init(wheel_e idx);
void wheels_init(void);

// 工具函数

void wheel_stop(wheel_e idx);
void wheel_forward(wheel_e idx);
void wheel_backward(wheel_e idx);

// 功能函数

void wheel_pwm_set(wheel_e idx, s16 pwm);


#endif
