#include "pid.h"

_pid pidValue[5]; // PID 控制器数组

/**
 * \brief 初始化 PID 控制器
 * \param pid PID 控制器结构体指针
 * \param Kp 比例系数
 * \param Ki 积分系数
 * \param Kd 微分系数
 */
void pid_init(_pid *pid, float Kp, float Ki, float Kd)
{
    pid->err = 0.0;
    pid->err_last = 0.0;
    pid->integral = 0.0;
    pid->output = 0.0;
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
}

/**
 * \brief 初始化单神经元 PID 控制器
 * \param vPID 神经元 PID 控制器结构体指针
 * \param vMax 过程变量的最大值（量程范围）
 * \param vMin 过程变量的最小值（量程范围）
 */
void neural_pid_init(_pid2 *vPID, float vMax, float vMin)
{
    vPID->setpoint = vMin; /* 设定值 */

    vPID->kcoef = 0.12; /* 神经元输出比例 */
    vPID->kp = 0.4;     /* 比例学习速度 */
    vPID->ki = 0.35;    /* 积分学习速度 */
    vPID->kd = 0.4;     /* 微分学习速度 */

    vPID->lasterror = 0.0; /* 前一拍偏差 */
    vPID->preerror = 0.0;  /* 前两拍偏差 */
    vPID->result = vMin;   /* PID 控制器结果 */
    vPID->output = 0.0;    /* 输出值，百分比 */

    vPID->maximum = vMax;                    /* 输出值上限 */
    vPID->minimum = vMin;                    /* 输出值下限 */
    vPID->deadband = (vMax - vMin) * 0.0005; /* 死区 */

    vPID->wp = 0.10; /* 比例加权系数 */
    vPID->wi = 0.10; /* 积分加权系数 */
    vPID->wd = 0.10; /* 微分加权系数 */
}

/**
 * \brief PID 控制算法
 * \param pid PID 控制器结构体指针
 * \param set_value 目标值
 * \param actual_value 实际值
 * \return 返回计算后的 PWM 比较值
 */
u16 pid_action(_pid *pid, float set_value, float actual_value)
{
    pid->err = set_value - actual_value;                                                               // 计算当前偏差
    pid->integral += pid->err;                                                                         // 计算积分值
    pid->output = pid->Kp * pid->err + pid->Ki * pid->integral + pid->Kd * (pid->err - pid->err_last); // 计算输出值
    pid->err_last = pid->err;                                                                          // 更新上一次偏差

    // 限制范围
    if (pidIdx == 1 || pidIdx == 2) // 仅对左右轮 PID 输出进行限制
    {
        if (pid->output > 1000)
            pid->output = 1000;
        else if (pid->output < -1000)
            pid->output = -1000;
    }

    return (u16)pid->output;
}

/**
 * \brief 神经网络 PID 控制算法
 * \param vPID 神经元 PID 控制器结构体指针
 * \param pv 过程测量值，对象响应的测量数据，用于控制反馈
 * \note  函数为增量式 PID 控制
 */
void neural_pid_action(_pid2 *vPID, float pv)
{
    float x[3];
    float w[3];
    float sabs;
    float error;
    float result;
    float deltaResult;

    error = vPID->setpoint - pv;
    result = vPID->result;
    if (fabs(error) > vPID->deadband)
    {
        x[0] = error;
        x[1] = error - vPID->lasterror;
        x[2] = error - vPID->lasterror * 2 + vPID->preerror;

        sabs = fabs(vPID->wi) + fabs(vPID->wp) + fabs(vPID->wd);
        w[0] = vPID->wi / sabs;
        w[1] = vPID->wp / sabs;
        w[2] = vPID->wd / sabs;

        deltaResult = (w[0] * x[0] + w[1] * x[1] + w[2] * x[2]) * vPID->kcoef;
    }
    else
    {
        deltaResult = 0;
    }

    result = result + deltaResult;
    if (result > vPID->maximum)
    {
        result = vPID->maximum;
    }
    if (result < vPID->minimum)
    {
        result = vPID->minimum;
    }
    vPID->result = result;
    vPID->output = (vPID->result - vPID->minimum) * 100 / (vPID->maximum - vPID->minimum);

    // 单神经元学习
    neure_learning_rules(vPID, error, result, x);

    vPID->preerror = vPID->lasterror;
    vPID->lasterror = error;
}

/**
 * \brief 单神经元学习规则函数
 * \param vPID 神经元 PID 控制器结构体指针
 * \param zk 当前误差
 * \param uk 当前输出
 * \param xi 输入向量
 */
static void neure_learning_rules(_pid2 *vPID, float zk, float uk, float *xi)
{
    vPID->wi = vPID->wi + vPID->ki * zk * uk * xi[0];
    vPID->wp = vPID->wp + vPID->kp * zk * uk * xi[1];
    vPID->wd = vPID->wd + vPID->kd * zk * uk * xi[2];
}
