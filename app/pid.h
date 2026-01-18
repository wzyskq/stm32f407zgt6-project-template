#ifndef __PID_H
#define __PID_H

#include "main.h"

/* Global Macro ------------------------------------------------------------ */

/* Global Types ------------------------------------------------------------ */

typedef struct
{
    float err;        // 当前偏差值
    float err_last;   // 上一次的偏差值
    float integral;   // 积分值
    float kp, ki, kd; // 比例、积分、微分系数
    float output;     // 控制输出
} pid_t;

/* Global Variables -------------------------------------------------------- */

extern u8 pidIdx;       // 当前使用的 PID 控制器索引
extern pid_t pidValue[]; // PID 控制器数组

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void pid_init(pid_t *pid, float kp, float ki, float kd);

// 功能函数

float pid_action(pid_t *pid, float set_value, float actual_value);

// 算法函数

s16 exp_step_core(u8 tarIdx, u8 n, u8 exp, u8 e, s16 Vt, s16 V0);

#endif
