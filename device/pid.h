#ifndef __PID_H
#define __PID_H

#include "main.h"

typedef struct
{
    float err;         // 当前偏差值
    float err_last;    // 上一次的偏差值
    float integral;    // 积分值
    float Kp, Ki, Kd;  // 比例、积分、微分系数
    float output;      // 控制输出
} _pid;

typedef struct
{
    float setpoint;  /* 设定值 */
    float kcoef;     /* 神经元输出比例 */
    float kp;        /* 比例学习速度 */
    float ki;        /* 积分学习速度 */
    float kd;        /* 微分学习速度 */
    float lasterror; /* 前一拍偏差 */
    float preerror;  /* 前两拍偏差 */
    float deadband;  /* 死区 */
    float result;    /* 输出值 */
    float output;    /* 百分比输出值 */
    float maximum;   /* 输出值的上限 */
    float minimum;   /* 输出值的下限 */
    float wp;        /* 比例加权系数 */
    float wi;        /* 积分加权系数 */
    float wd;        /* 微分加权系数 */
} _pid2;

extern _pid pidValue[]; // PID 控制器数组
extern _pid2 pidVal2[]; // PID 控制器数组

// 通用 PID 控制器

void pid_init(_pid *pid, float Kp, float Ki, float Kd);
u16 pid_action(_pid *pid, float set_value, float actual_value);

// [神经网络 PID 控制器](https://www.cnblogs.com/ren-jiong/p/15136615.html)

void neural_pid_init(_pid2 *vPID, float vMax, float vMin);
void neural_pid_action(_pid2 *vPID, float pv);
static void neure_learning_rules(_pid2 *vPID, float zk, float uk, float *xi);

#endif
