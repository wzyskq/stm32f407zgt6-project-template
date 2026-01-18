#include "pid.h"

/* Global Variables -------------------------------------------------------- */

u8 pidIdx = 0;     // 当前使用的 PID 控制器索引
pid_t pidValue[5]; // PID 控制器数组

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化 PID 控制器
 * \param[in]  pid PID 控制器结构体指针
 * \param[in]  Kp 比例系数
 * \param[in]  Ki 积分系数
 * \param[in]  Kd 微分系数
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

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief      PID 控制算法
 * \param[in]  pid PID 控制器结构体指针
 * \param[in]  set_value 目标值
 * \param[in]  actual_value 实际值
 * \return     返回计算后的 PWM 比较值
 */
float pid_action(pid_t *pid, float set_value, float actual_value)
{
    pid->err = set_value - actual_value; // 计算当前偏差
    if (pidIdx == pidObj_dir) {          // 方向环特殊处理，计算最短偏差
        if (pid->err <= -180.0f) pid->err += 360.0f;
        if (pid->err > 180.0f) pid->err -= 360.0f;
    }
    pid->integral += pid->err;                                                                           // 计算积分值
    pid->output   = pid->kp * pid->err + pid->ki * pid->integral + pid->kd * (pid->err - pid->err_last); // 计算输出值
    pid->err_last = pid->err;                                                                            // 更新上一次偏差

    // 速度环限幅
    if (pidIdx == pidObj_whlLt || pidIdx == pidObj_whlRt) {
        if (pid->output > 1000)
            pid->output = 1000;
        else if (pid->output < -1000)
            pid->output = -1000;
    }

    return pid->output;
}

/* ******************** 功能函数 */

/*





*/

/* 算法函数 ******************** */

/******************************************************************
 * \brief      指数步进算法
 * \param[in]  tarIdx  目标编号
 * \param[in]  n       步进段数，1..255
 * \param[in]  exp     步进指数，1..255
 * \param[in]  e       最大误差，0..255
 * \param[in]  Vt      目标速度，-32768..32767
 * \param[in]  V0      当前速度，-32768..32767
 * \return     返回下一步目标速度
 *
 * \warning     - exp 参数过大可能导致计算缓慢或者中断卡死
 *              - exp != 1 时，n 不宜过大，否则步进时间过长
 */
s16 exp_step_core(u8 tarIdx, u8 n, u8 exp, u8 e, s16 Vt, s16 V0)
{
    static u8 sbsFlag[EXP_ARR_LEN] = {0}; // 步进标志位
    static u8 n0[EXP_ARR_LEN]      = {0}; // 原始分段备份
    static u8 exp0[EXP_ARR_LEN]    = {0}; // 原始指数备份
    static s16 Vt0[EXP_ARR_LEN]    = {0}; // 目标速度备份

    static s16 x[EXP_ARR_LEN]  = {0}; // 原始距离差
    static s16 s[EXP_ARR_LEN]  = {0}; // 步进状态
    static s16 Vn[EXP_ARR_LEN] = {0}; // 下一步目标速度

    // 可扩展指数数组
    static u8 arrIdx            = 0;   // 指数数组索引
    static float expArr[1][256] = {0}; // 指数记忆化存储数组（速度环 + 方向环）

    // 步进实现
    if (Vt0[tarIdx] != Vt || n0[tarIdx] != n || exp0[tarIdx] != exp) {
        if (exp0[tarIdx] != exp) {
            for (u8 i = 0; i <= n0[tarIdx]; i++)
                expArr[arrIdx][i] = 0;
            exp0[tarIdx] = exp;
        }

        n0[tarIdx]      = n;
        Vt0[tarIdx]     = Vt;
        x[tarIdx]       = Vt - V0;
        Vn[tarIdx]      = V0;
        s[tarIdx]       = 0;
        sbsFlag[tarIdx] = 1;
    }
    if (sbsFlag[tarIdx]) {
        if (abs16(Vn[tarIdx] - V0) <= e) {
            s[tarIdx]++;
            if (s[tarIdx] == n) {
                Vn[tarIdx]      = Vt;
                sbsFlag[tarIdx] = 0;
            } else {
                u8 a = n + 1 - s[tarIdx];
                u8 b = n - s[tarIdx];
                if (!expArr[arrIdx][a]) expArr[arrIdx][a] = powf(a, exp);
                if (!expArr[arrIdx][b]) expArr[arrIdx][b] = powf(b, exp);
                Vn[tarIdx] += (s16)(x[tarIdx] * (expArr[arrIdx][a] - expArr[arrIdx][b]) / expArr[arrIdx][n]);
            }
        }
    } else {
        Vn[tarIdx] = Vt;
    }

    // 调试变量赋值
    if (tarIdx == 0) {
        tWhlS  = s[0];
        tWhlVn = Vn[0];
    }

    return Vn[tarIdx];
}

//
// /******************************************************************
//  * \brief      指数步进算法
//  * \param[in]  tarIdx  目标编号，0..1
//  * \param[in]  n       步进段数，1..255
//  * \param[in]  exp     步进指数，1..4
//  * \param[in]  e       最大误差，0..255
//  * \param[in]  Vt      目标速度，-32768..32767
//  * \param[in]  V0      当前速度，-32768..32767
//  * \return     返回下一步目标速度
//  */
// s16 exp_step_core(u8 tarIdx, u8 n, u8 exp, u8 e, s16 Vt, s16 V0)
// {
//     static s16 l[2]   = {0}; // 步进单位长度
//     static s16 Vt0[2] = {0}; // 目标速度备份
//     static s16 Vn[2]  = {0}; // 下一步目标速度
//     static s16 s[2]   = {0}; // 步进状态
//
//     static u8 sbsFlag[2] = {0}; // 步进标志位
//
//     // 步进实现
//     if (Vt0[tarIdx] != Vt) {
//         float diff      = (float)(Vt - V0);
//         float div       = pow(n, exp);
//         l[tarIdx]       = (diff > 0) ? floor(diff / div) : ceil(diff / div);
//         Vt0[tarIdx]     = Vt;
//         Vn[tarIdx]      = V0;
//         s[tarIdx]       = 0;
//         sbsFlag[tarIdx] = 1;
//     }
//     if (sbsFlag[tarIdx]) {
//         if (abs16(Vn[tarIdx] - V0) <= e) {
//             s[tarIdx]++;
//             if (s[tarIdx] == n) {
//                 Vn[tarIdx]      = Vt;
//                 sbsFlag[tarIdx] = 0;
//             } else {
//                 u8 k = n - s[tarIdx];
//                 if (exp == 1)
//                     Vn[tarIdx] += l[tarIdx];
//                 else if (exp == 2)
//                     Vn[tarIdx] += l[tarIdx] * (2 * k + 1);
//                 else if (exp == 3)
//                     Vn[tarIdx] += l[tarIdx] * (3 * k * k + 3 * k + 1);
//                 else if (exp == 4)
//                     Vn[tarIdx] += l[tarIdx] * (4 * k * k * k - 6 * k * k + 4 * k + 1);
//             }
//         }
//     } else {
//         Vn[tarIdx] = Vt;
//     }
//
//     // 调试变量赋值
//     if (tarIdx == 0      ) {
//         tWhlS  = s[0];
//         tWhlVn = Vn[0];
//     }
//
//     return Vn[tarIdx];
// }
//

/* ******************** 算法函数 */
