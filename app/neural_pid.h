// #ifndef __NEURAL_PID_H
// #define __NEURAL_PID_H
// 
// #include "main.h"
// 
// typedef struct
// {
//     float setpoint;  /* 设定值 */
//     float kcoef;     /* 神经元输出比例 */
//     float kp;        /* 比例学习速度 */
//     float ki;        /* 积分学习速度 */
//     float kd;        /* 微分学习速度 */
//     float lasterror; /* 前一拍偏差 */
//     float preerror;  /* 前两拍偏差 */
//     float deadband;  /* 死区 */
//     float result;    /* 输出值 */
//     float output;    /* 百分比输出值 */
//     float maximum;   /* 输出值的上限 */
//     float minimum;   /* 输出值的下限 */
//     float wp;        /* 比例加权系数 */
//     float wi;        /* 积分加权系数 */
//     float wd;        /* 微分加权系数 */
// } pid_t;
// 
// extern pid_t pidValue[];
// extern u8 pidIdx;
// extern u16 pidErr;
// 
// // 通用 PID 控制器
// 
// // [神经网络 PID 控制器](https://www.cnblogs.com/ren-jiong/p/15136615.html)
// 
// void neural_pid_init(pid_t *vPID, float vMax, float vMin);
// void neural_pid_action(pid_t *vPID, float pv);
// static void neure_learning_rules(pid_t *vPID, float zk, float uk, float *xi);
// 
// #endif
