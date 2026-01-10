#include "pid.h"

/* Global Variables -------------------------------------------------------- */

u8 pidIdx = 0;     // 当前使用的 PID 控制器索引
pid_t pidValue[5]; // PID 控制器数组

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief 初始化 PID 控制器
 * \param pid PID 控制器结构体指针
 * \param Kp 比例系数
 * \param Ki 积分系数
 * \param Kd 微分系数
 */
void pid_init(pid_t *pid, float kp, float ki, float kd)
{
    pid->err      = 0.0;
    pid->err_last = 0.0;
    pid->integral = 0.0;
    pid->output   = 0.0;
    pid->kp       = kp;
    pid->ki       = ki;
    pid->kd       = kd;
}

/******************************************************************
 * \brief PID 控制算法
 * \param pid PID 控制器结构体指针
 * \param set_value 目标值
 * \param actual_value 实际值
 * \return 返回计算后的 PWM 比较值
 */
float pid_action(pid_t *pid, float set_value, float actual_value)
{
    pid->err = set_value - actual_value;                                                                 // 计算当前偏差
    pid->integral += pid->err;                                                                           // 计算积分值
    pid->output   = pid->kp * pid->err + pid->ki * pid->integral + pid->kd * (pid->err - pid->err_last); // 计算输出值
    pid->err_last = pid->err;                                                                            // 更新上一次偏差

    // 速度环限幅
    if (pidIdx == 1 || pidIdx == 2) {
        if (pid->output > 1000)
            pid->output = 1000;
        else if (pid->output < -1000)
            pid->output = -1000;
    }

    return pid->output;
}
