#ifndef __PID_H
#define __PID_H

typedef struct
{
    float SetValue;    // 目标值（目标坐标）
    float ActualValue; // 实际值（当前坐标）
    float err;         // 当前偏差值
    float err_last;    // 上一次的偏差值
    float integral;    // 积分值
    float Kp, Ki, Kd;  // 比例、积分、微分系数
    float output;      // 控制输出（舵机角度）
} _pid;

extern _pid pidValue[2]; // PID 控制器数组

void pid_init(_pid *pid, float Kp, float Ki, float Kd);
WORD pid_action(_pid *pid, float set_value, float actual_value);

#endif
