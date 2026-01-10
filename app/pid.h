#ifndef __PID_H
#define __PID_H

#include "main.h"

/* Exported Macro ---------------------------------------------------------- */

/* Exported Types ---------------------------------------------------------- */

typedef struct
{
    float err;        // 当前偏差值
    float err_last;   // 上一次的偏差值
    float integral;   // 积分值
    float kp, ki, kd; // 比例、积分、微分系数
    float output;     // 控制输出
} pid_t;

/* Exported Variables ------------------------------------------------------ */

extern u8 pidIdx;       // 当前使用的 PID 控制器索引
extern pid_t pidValue[]; // PID 控制器数组

/* Exported Functions ------------------------------------------------------ */

void pid_init(pid_t *pid, float kp, float ki, float kd);
float pid_action(pid_t *pid, float set_value, float actual_value);

#endif
