#include "main.h"

_pid pidValue[2]; // PID 控制器数组

/**
 * @brief 初始化 PID 控制器
 * @param pid PID 控制器结构体指针
 * @param Kp 比例系数
 * @param Ki 积分系数
 * @param Kd 微分系数
 */
void pid_init(_pid *pid, float Kp, float Ki, float Kd)
{
    pid->SetValue = 0.0;
    pid->ActualValue = 0.0;
    pid->err = 0.0;
    pid->err_last = 0.0;
    pid->integral = 0.0;
    pid->output = 0.0;
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

/**
 * @brief PID 控制算法
 * @param pid PID 控制器结构体指针
 * @param set_value 目标值（目标速度）
 * @param actual_value 实际值（当前速度）
 * @return 返回计算后的 PWM 比较值
 */
WORD pid_action(_pid *pid, float set_value, float actual_value)
{
    pid->SetValue = set_value;
    pid->ActualValue = actual_value;

    pid->err = pid->ActualValue - pid->SetValue;
    pid->integral += pid->err;
    pid->output = pid->Kp * pid->err + pid->Ki * pid->integral + pid->Kd * (pid->err - pid->err_last);
    pid->err_last = pid->err;

    // // 限制范围
    // if (pid->output < 0)
    //     pid->output = 0;
    // if (pid->output > 1000)
    //     pid->output = 1000;

    return (WORD)pid->output;
}
