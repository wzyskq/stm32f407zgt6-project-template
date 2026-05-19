/******************************************************************
 ** \file   X_V2.c
 **
 ** \author Yiiry
 **
 ** \brief  本文件（X_V2.c/.h）修改自张大头闭环步进电机驱动，主要功能为发送控制/读取命令
 **
 ** \post   可配合笔者封装的 zdt_api.c/.h 使用实现电机返回数据包的异步接收和解析
 **
 ** \note   相较于原版，主要修改内容包括：
 **
 **         - 采用 Doxygen 风格格式化函数注释，并用将类型名更换为更简洁的别名（u8，u16，u32 等等）.
 **
 **         - 函数增加形参串口号来适配多串口控制，并用 serial_send_x_v2_cmd() 统一命令发送接口（若需移植直接改该函数即可）.
 **
 **         推荐配合 zdt_api.c/.h 和 zdt_pro.c/.h 使用，具体使用方法请参考文件中的函数注释和示例.
 */

#include "X_V2.h"

#if !defined(ZDT_EMM_V5) && defined(ZDT_X_V2) // 仅当定义 ZDT_X_V2 时编译当前文件内容

/**********************************************************
 ***    X_V2步进闭环控制例程
 ***    编写作者：ZHANGDATOU
 ***    技术支持：张大头闭环伺服
 ***    淘宝店铺：https://zhangdatou.taobao.com
 ***    CSDN博客：http s://blog.csdn.net/zhangdatou666
 ***    qq交流群：262438510
 **********************************************************/

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

__IO u16 MMCL_count = 0, MMCL_cmd[MMCL_LEN] = {0};

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief      发送 X_V2.0 命令
 * \param[in]  idx  串口索引
 * \param[in]  cmd  命令字节数组
 * \param[in]  len  命令字节长度
 *
 * \warning    严禁在此函数内/紧邻后部调用其他串口，否则会导致系统死机、串口端口混淆死机等一系列问题！！！
 */
static void serial_send_x_v2_cmd(srl_e idx, __IO u8 *cmd, u16 len)
{
    // 这里一定不能加其他串口

    while (zdtTvFlg); // ******** 可选行，需配合 zdt_api.c/.h 使用，等待清零 ********

    zdtTvFlg = true; // ******** 可选行，需配合 zdt_api.c/.h 使用，设置发送标志位 ********

    zdtTvTag[0] = cmd[0]; // ******** 可选行，需配合 zdt_api.c/.h 使用，记录地址信息 ********
    zdtTvTag[1] = cmd[1]; // ******** 可选行，需配合 zdt_api.c/.h 使用，记录功能码信息 ********

    u8 i;
    for (i = 0; i < len; i++)
        serial_send_byte(idx, cmd[i]);

    zdtTimeoutCnt = 0;    // ******** 可选行，需配合 zdt_api.c/.h 使用，重置超时计数 ********
    zdtTimeoutFlg = true; // ******** 可选行，需配合 zdt_api.c/.h 使用，设置超时标志位，开始计时 ********
}

/*





*/

/**********************************************************
*** 触发动作命令
**********************************************************/

/* 触发动作命令 ******************** */

/******************************************************************
 * \brief      触发编码器校准
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Trig_Encoder_Cal(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x06; // 功能码
    cmd[2] = 0x45; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      重启电机（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Reset_Motor(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x08; // 功能码
    cmd[2] = 0x97; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      将当前位置清零
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Reset_CurPos_To_Zero(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0A; // 功能码
    cmd[2] = 0x6D; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      解除堵转保护
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Reset_Clog_Pro(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0E; // 功能码
    cmd[2] = 0x52; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      恢复出厂设置
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Restore_Motor(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0F; // 功能码
    cmd[2] = 0x5F; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/* ******************** 触发动作命令 */

/*





*/

/**********************************************************
*** 运动控制命令
**********************************************************/

/* 运动控制命令 ******************** */

/******************************************************************
 * \brief      多电机命令（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Multi_Motor_Cmd(srl_e idx, u8 addr)
{
    u16 i = 0, j = 0, len = 0;
    __IO static u8 cmd[MMCL_LEN] = {0};

    // 多电机命令长度大于0
    if (MMCL_count > 0) {
        // 多电机命令的总字节数
        len = MMCL_count + 5;

        // 装载命令
        cmd[0] = addr;           // 地址
        cmd[1] = 0xAA;           // 功能码
        cmd[2] = (u8)(len >> 8); // 总字节数高8位
        cmd[3] = (u8)(len);      // 总字节数低8位
        for (i = 0, j = 4; i < MMCL_count; i++, j++) { cmd[j] = MMCL_cmd[i]; }
        cmd[j] = 0x6B;
        ++j; // 校验字节

        // 发送命令
        serial_send_x_v2_cmd(idx, cmd, j);
        MMCL_count = 0;
    } else {
        MMCL_count = 0;
    }
}

/******************************************************************
 * \brief      使能信号控制
 * \param[in]  addr  电机地址
 * \param[in]  state 使能状态，true为使能电机，false为关闭电机
 * \param[in]  snF   多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_En_Control(srl_e idx, u8 addr, bool state, bool snF)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;      // 地址
    cmd[1] = 0xF3;      // 功能码
    cmd[2] = 0xAB;      // 辅助码
    cmd[3] = (u8)state; // 使能状态
    cmd[4] = snF;       // 多机同步运动标志
    cmd[5] = 0x6B;      // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      力矩模式
 * \param[in]  addr 电机地址
 * \param[in]  sign 符号（方向），0为正，1为负
 * \param[in]  t_ramp 电流斜率(Ma/s)，范围0 - 65535Ma/s
 * \param[in]  torque 力矩电流(Ma)，范围0 - 6000Ma
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Torque_Control(srl_e idx, u8 addr, u8 sign, u16 t_ramp, u16 torque, bool snF)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;              // 地址
    cmd[1] = 0xF5;              // 功能码
    cmd[2] = sign;              // 符号（方向）
    cmd[3] = (u8)(t_ramp >> 8); // 电流斜率(Ma/s)
    cmd[4] = (u8)(t_ramp >> 0);
    cmd[5] = (u8)(torque >> 8); // 力矩电流(Ma)
    cmd[6] = (u8)(torque >> 0);
    cmd[7] = snF;  // 多机同步标志
    cmd[8] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 9);
}

/******************************************************************
 * \brief      力矩模式限速控制（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  sign 符号（方向），0为正，1为负
 * \param[in]  t_ramp 电流斜率(Ma/s)，范围0 - 65535Ma/s
 * \param[in]  torque 力矩电流(Ma)，范围0 - 6000Ma
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \param[in]  maxVel 最大速度(RPM)，范围0.0 - 3000.0RPM
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Torque_LV_Control(srl_e idx, u8 addr, u8 sign, u16 t_ramp, u16 torque, bool snF, float maxVel)
{
    __IO static u8 cmd[16] = {0};
    u16 v                  = 0;

    // 将速度放大10倍发送过去
    v = (u16)ABS(maxVel * 10.0f);

    // 装载命令
    cmd[0]  = addr;              // 地址
    cmd[1]  = 0xC5;              // 功能码
    cmd[2]  = sign;              // 符号（方向）
    cmd[3]  = (u8)(t_ramp >> 8); // 电流斜率(Ma/s)
    cmd[4]  = (u8)(t_ramp >> 0);
    cmd[5]  = (u8)(torque >> 8); // 力矩电流(Ma)
    cmd[6]  = (u8)(torque >> 0);
    cmd[7]  = snF;          // 多机同步标志
    cmd[8]  = (u8)(v >> 8); // 最大速度(RPM)
    cmd[9]  = (u8)(v >> 0);
    cmd[10] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 11);
}

/******************************************************************
 * \brief      速度模式
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，1为CCW
 * \param[in]  acc 加速度(RPM/s)，范围0 - 65535RPM/s
 * \param[in]  vel 速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Vel_Control(srl_e idx, u8 addr, u8 dir, u16 acc, float vel, bool snF)
{
    __IO static u8 cmd[16] = {0};
    u16 v                  = 0;

    // 将速度放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0xF6;           // 功能码
    cmd[2] = dir;            // 符号（方向）
    cmd[3] = (u8)(acc >> 8); // 加速度(RPM/s)
    cmd[4] = (u8)(acc >> 0);
    cmd[5] = (u8)(v >> 8); // 速度(RPM)
    cmd[6] = (u8)(v >> 0);
    cmd[7] = snF;  // 多机同步运动标志
    cmd[8] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 9);
}

/******************************************************************
 * \brief      速度模式限电流控制（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，1为CCW
 * \param[in]  acc 加速度(RPM/s)，范围0 - 65535RPM/s
 * \param[in]  vel 速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \param[in]  maxCur 最大电流(mA)，范围0 - 6000mA
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Vel_LC_Control(srl_e idx, u8 addr, u8 dir, u16 acc, float vel, bool snF, u16 maxCur)
{
    __IO static u8 cmd[16] = {0};
    u16 v                  = 0;

    // 将速度放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);

    // 装载命令
    cmd[0]  = addr;           // 地址
    cmd[1]  = 0xC6;           // 功能码
    cmd[2]  = dir;            // 符号（方向）
    cmd[3]  = (u8)(acc >> 8); // 加速度(RPM/s)
    cmd[4]  = (u8)(acc >> 0);
    cmd[5]  = (u8)(v >> 8); // 速度(RPM)
    cmd[6]  = (u8)(v >> 0);
    cmd[7]  = snF;               // 多机同步运动标志
    cmd[8]  = (u8)(maxCur >> 8); // 最大电流(mA)
    cmd[9]  = (u8)(maxCur >> 0);
    cmd[10] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 11);
}

/******************************************************************
 * \brief      直通限速位置模式
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，1为CCW
 * \param[in]  vel 运动速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  pos 位置角度(°)，范围0.0°- (2^32 - 1) / 10°
 * \param[in]  raf 相位/绝对运动标志，0为相对上一次输入目标位置，1为绝对位置，2为相对当前实时位置
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Bypass_Pos_LV_Control(srl_e idx, u8 addr, u8 dir, float vel, float pos, u8 raf, bool snF)
{
    u8 cmd[16] = {0};
    u16 v      = 0;
    u32 p      = 0;

    // 将速度和位置放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);
    p = (u32)ABS(pos * 10.0f);

    // 装载命令
    cmd[0]  = addr;         // 地址
    cmd[1]  = 0xFB;         // 功能码
    cmd[2]  = dir;          // 符号（方向）
    cmd[3]  = (u8)(v >> 8); // 运动速度(RPM)
    cmd[4]  = (u8)(v >> 0);
    cmd[5]  = (u8)(p >> 24); // 位置角度
    cmd[6]  = (u8)(p >> 16);
    cmd[7]  = (u8)(p >> 8);
    cmd[8]  = (u8)(p >> 0);
    cmd[9]  = raf;  // 相位/绝对运动标志
    cmd[10] = snF;  // 多机同步运动标志
    cmd[11] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 12);
}

/******************************************************************
 * \brief      直通限速位置模式限电流控制
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，1为CCW
 * \param[in]  vel 运动速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  pos 位置角度(°)，范围0.0°- (2^32 - 1) / 10°
 * \param[in]  raf 相位/绝对运动标志，0为相对上一次输入目标位置，1为绝对位置，2为相对当前实时位置
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \param[in]  maxCur 最大电流(mA)，范围0 - 6000mA
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Bypass_Pos_LV_LC_Control(srl_e idx, u8 addr, u8 dir, float vel, float pos, u8 raf, bool snF, u16 maxCur)
{
    u8 cmd[16] = {0};
    u16 v      = 0;
    u32 p      = 0;

    // 将速度和位置放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);
    p = (u32)ABS(pos * 10.0f);

    // 装载命令
    cmd[0]  = addr;         // 地址
    cmd[1]  = 0xCB;         // 功能码
    cmd[2]  = dir;          // 符号（方向）
    cmd[3]  = (u8)(v >> 8); // 运动速度(RPM)
    cmd[4]  = (u8)(v >> 0);
    cmd[5]  = (u8)(p >> 24); // 位置角度
    cmd[6]  = (u8)(p >> 16);
    cmd[7]  = (u8)(p >> 8);
    cmd[8]  = (u8)(p >> 0);
    cmd[9]  = raf;               // 相位/绝对运动标志
    cmd[10] = snF;               // 多机同步运动标志
    cmd[11] = (u8)(maxCur >> 8); // 最大电流(mA)
    cmd[12] = (u8)(maxCur >> 0);
    cmd[13] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 14);
}

/******************************************************************
 * \brief      梯形曲线加减速位置模式控制
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，其余值为CCW
 * \param[in]  acc 加速加速度(RPM/s)，0为CW，其余值为CCW
 * \param[in]  dec 减速加速度(RPM/s)，0为CW，其余值为CCW
 * \param[in]  vel 最大速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  pos 位置(°)，范围0.0°- (2^32 - 1)°
 * \param[in]  raf 相位/绝对运动标志，0为相对上一次输入目标位置，1为绝对位置，2为相对当前实时位置
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Traj_Pos_Control(srl_e idx, u8 addr, u8 dir, u16 acc, u16 dec, float vel, float pos, u8 raf, bool snF)
{
    u8 cmd[32] = {0};
    u16 v      = 0;
    u32 p      = 0;

    // 将速度和位置放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);
    p = (u32)ABS(pos * 10.0f);

    // 装载命令
    cmd[0]  = addr;           // 地址
    cmd[1]  = 0xFD;           // 功能码
    cmd[2]  = dir;            // 符号（方向）
    cmd[3]  = (u8)(acc >> 8); // 加速加速度(RPM/s)
    cmd[4]  = (u8)(acc >> 0);
    cmd[5]  = (u8)(dec >> 8); // 减速加速度(RPM/s)
    cmd[6]  = (u8)(dec >> 0);
    cmd[7]  = (u8)(v >> 8); // 最大速度(RPM)
    cmd[8]  = (u8)(v >> 0);
    cmd[9]  = (u8)(p >> 24); // 位置角度
    cmd[10] = (u8)(p >> 16);
    cmd[11] = (u8)(p >> 8);
    cmd[12] = (u8)(p >> 0);
    cmd[13] = raf;  // 相位/绝对运动标志
    cmd[14] = snF;  // 多机同步运动标志
    cmd[15] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 16);
}

/******************************************************************
 * \brief      梯形曲线加减速位置模式限电流控制（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，其余值为CCW
 * \param[in]  acc 加速加速度(RPM/s)，0为CW，其余值为CCW
 * \param[in]  dec 减速加速度(RPM/s)，0为CW，其余值为CCW
 * \param[in]  vel 最大速度(RPM)，范围0.0 - 3000.0RPM
 * \param[in]  pos 位置(°)，范围0.0°- (2^32 - 1)°
 * \param[in]  raf 相位/绝对运动标志，0为相对上一次输入目标位置，1为绝对位置，2为相对当前实时位置
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \param[in]  maxCur 最大电流(mA)，范围0 - 6000mA
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Traj_Pos_LC_Control(srl_e idx, u8 addr, u8 dir, u16 acc, u16 dec, float vel, float pos, u8 raf, bool snF, u16 maxCur)
{
    u8 cmd[32] = {0};
    u16 v      = 0;
    u32 p      = 0;

    // 将速度和位置放大10倍发送过去
    v = (u16)ABS(vel * 10.0f);
    p = (u32)ABS(pos * 10.0f);

    // 装载命令
    cmd[0]  = addr;           // 地址
    cmd[1]  = 0xCD;           // 功能码
    cmd[2]  = dir;            // 符号（方向）
    cmd[3]  = (u8)(acc >> 8); // 加速加速度(RPM/s)
    cmd[4]  = (u8)(acc >> 0);
    cmd[5]  = (u8)(dec >> 8); // 减速加速度(RPM/s)
    cmd[6]  = (u8)(dec >> 0);
    cmd[7]  = (u8)(v >> 8); // 最大速度(RPM)
    cmd[8]  = (u8)(v >> 0);
    cmd[9]  = (u8)(p >> 24); // 位置角度
    cmd[10] = (u8)(p >> 16);
    cmd[11] = (u8)(p >> 8);
    cmd[12] = (u8)(p >> 0);
    cmd[13] = raf;               // 相位/绝对运动标志
    cmd[14] = snF;               // 多机同步运动标志
    cmd[15] = (u8)(maxCur >> 8); // 最大电流(mA)
    cmd[16] = (u8)(maxCur >> 0);
    cmd[17] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 18);
}

/******************************************************************
 * \brief      立即停止
 * \param[in]  addr 电机地址
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Stop_Now(srl_e idx, u8 addr, bool snF)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFE; // 功能码
    cmd[2] = 0x98; // 辅助码
    cmd[3] = snF;  // 多机同步运动标志
    cmd[4] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 5);
}

/******************************************************************
 * \brief      多机同步运动
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Synchronous_motion(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFF; // 功能码
    cmd[2] = 0x66; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/* ******************** 运动控制命令 */

/*





*/

/**********************************************************
*** 原点回零命令
**********************************************************/

/* 原点回零命令 ******************** */

/******************************************************************
 * \brief      设置单圈回零的零点位置
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Origin_Set_O(srl_e idx, u8 addr, bool svF)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x93; // 功能码
    cmd[2] = 0x88; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 5);
}

/******************************************************************
 * \brief      触发回零
 * \param[in]  addr 电机地址
 * \param[in]  o_mode 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Origin_Trigger_Return(srl_e idx, u8 addr, u8 o_mode, bool snF)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;   // 地址
    cmd[1] = 0x9A;   // 功能码
    cmd[2] = o_mode; // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[3] = snF;    // 多机同步运动标志，false为不启用，true为启用
    cmd[4] = 0x6B;   // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 5);
}

/******************************************************************
 * \brief      强制中断并退出回零
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Origin_Interrupt(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x9C; // 功能码
    cmd[2] = 0x48; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      读取回零参数
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Origin_Read_Params(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x22; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改回零参数
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  o_mode 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * \param[in]  o_dir 回零方向，0为CW，其余值为CCW
 * \param[in]  o_vel 回零速度，单位：RPM（转/分钟）
 * \param[in]  o_tm 回零超时时间，单位：毫秒
 * \param[in]  sl_vel 无限位碰撞回零检测转速，单位：RPM（转/分钟）
 * \param[in]  sl_ma 无限位碰撞回零检测电流，单位：Ma（毫安）
 * \param[in]  sl_ms 无限位碰撞回零检测时间，单位：Ms（毫秒）
 * \param[in]  potF 上电自动触发回零，false为不使能，true为使能
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Origin_Modify_Params(srl_e idx, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
    __IO static u8 cmd[32] = {0};

    // 装载命令
    cmd[0]  = addr;              // 地址
    cmd[1]  = 0x4C;              // 功能码
    cmd[2]  = 0xAE;              // 辅助码
    cmd[3]  = svF;               // 是否存储标志，false为不存储，true为存储
    cmd[4]  = o_mode;            // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[5]  = o_dir;             // 回零方向
    cmd[6]  = (u8)(o_vel >> 8);  // 回零速度(RPM)高8位字节
    cmd[7]  = (u8)(o_vel >> 0);  // 回零速度(RPM)低8位字节
    cmd[8]  = (u8)(o_tm >> 24);  // 回零超时时间(bit24 - bit31)
    cmd[9]  = (u8)(o_tm >> 16);  // 回零超时时间(bit16 - bit23)
    cmd[10] = (u8)(o_tm >> 8);   // 回零超时时间(bit8  - bit15)
    cmd[11] = (u8)(o_tm >> 0);   // 回零超时时间(bit0  - bit7 )
    cmd[12] = (u8)(sl_vel >> 8); // 无限位碰撞回零检测转速(RPM)高8位字节
    cmd[13] = (u8)(sl_vel >> 0); // 无限位碰撞回零检测转速(RPM)低8位字节
    cmd[14] = (u8)(sl_ma >> 8);  // 无限位碰撞回零检测电流(Ma)高8位字节
    cmd[15] = (u8)(sl_ma >> 0);  // 无限位碰撞回零检测电流(Ma)低8位字节
    cmd[16] = (u8)(sl_ms >> 8);  // 无限位碰撞回零检测时间(Ms)高8位字节
    cmd[17] = (u8)(sl_ms >> 0);  // 无限位碰撞回零检测时间(Ms)低8位字节
    cmd[18] = potF;              // 上电自动触发回零，false为不使能，true为使能
    cmd[19] = 0x6B;              // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 20);
}

/* ********************** 原点回零命令 */

/*





*/

/**********************************************************
*** 读取系统参数命令
**********************************************************/

/* 读取系统参数命令 ******************** */

/******************************************************************
 * \brief      定时返回信息命令（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  s 系统参数类型
 * \param[in]  time_ms 定时时间
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Auto_Return_Sys_Params_Timed(srl_e idx, u8 addr, zdtSysParams_e s, u16 time_ms)
{
    u8 i                   = 0;
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[i] = addr;
    ++i; // 地址

    cmd[i] = 0x11;
    ++i; // 功能码

    cmd[i] = 0x18;
    ++i; // 辅助码

    switch (s) // 信息功能码
    {
        case S_VBUS:
            cmd[i] = 0x24;
            ++i;
            break; // 读取总线电压
        case S_CBUS:
            cmd[i] = 0x26;
            ++i;
            break; // 读取总线电流
        case S_CPHA:
            cmd[i] = 0x27;
            ++i;
            break; // 读取相电流
        case S_ENCO:
            cmd[i] = 0x29;
            ++i;
            break; // 读取编码器原始值
        case S_CLKC:
            cmd[i] = 0x30;
            ++i;
            break; // 读取实时脉冲数
        case S_ENCL:
            cmd[i] = 0x31;
            ++i;
            break; // 读取经过线性化校准后的编码器值
        case S_CLKI:
            cmd[i] = 0x32;
            ++i;
            break; // 读取输入脉冲数
        case S_TPOS:
            cmd[i] = 0x33;
            ++i;
            break; // 读取电机目标位置
        case S_SPOS:
            cmd[i] = 0x34;
            ++i;
            break; // 读取电机实时设定的目标位置
        case S_VEL:
            cmd[i] = 0x35;
            ++i;
            break; // 读取电机实时转速
        case S_CPOS:
            cmd[i] = 0x36;
            ++i;
            break; // 读取电机实时位置
        case S_PERR:
            cmd[i] = 0x37;
            ++i;
            break; // 读取电机位置误差
        case S_VBAT:
            cmd[i] = 0x38;
            ++i;
            break; // 读取多圈编码器电池电压（Y42）
        case S_TEMP:
            cmd[i] = 0x39;
            ++i;
            break; // 读取电机实时温度（X42S/Y42）
        case S_FLAG:
            cmd[i] = 0x3A;
            ++i;
            break; // 读取电机状态标志位
        case S_OFLAG:
            cmd[i] = 0x3B;
            ++i;
            break; // 读取回零状态标志位
        case S_OAF:
            cmd[i] = 0x3C;
            ++i;
            break; // 读取电机状态标志位 + 回零状态标志位（X42S/Y42）
        case S_PIN:
            cmd[i] = 0x3D;
            ++i;
            break; // 读取引脚IO状态（X42S/Y42）
        default:
            break;
    }

    cmd[i] = (u8)(time_ms >> 8);
    ++i; // 定时时间
    cmd[i] = (u8)(time_ms >> 0);
    ++i;

    cmd[i] = 0x6B;
    ++i; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, i);
}

/******************************************************************
 * \brief      读取系统参数
 * \param[in]  addr 电机地址
 * \param[in]  s 系统参数类型
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Sys_Params(srl_e idx, u8 addr, zdtSysParams_e s)
{
    u8 i                   = 0;
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[i] = addr;
    ++i; // 地址

    switch (s) // 功能码
    {
        case S_VBUS:
            cmd[i] = 0x24;
            ++i;
            break; // 读取总线电压
        case S_CBUS:
            cmd[i] = 0x26;
            ++i;
            break; // 读取总线电流
        case S_CPHA:
            cmd[i] = 0x27;
            ++i;
            break; // 读取相电流
        case S_ENCO:
            cmd[i] = 0x29;
            ++i;
            break; // 读取编码器原始值
        case S_CLKC:
            cmd[i] = 0x30;
            ++i;
            break; // 读取实时脉冲数
        case S_ENCL:
            cmd[i] = 0x31;
            ++i;
            break; // 读取经过线性化校准后的编码器值
        case S_CLKI:
            cmd[i] = 0x32;
            ++i;
            break; // 读取输入脉冲数
        case S_TPOS:
            cmd[i] = 0x33;
            ++i;
            break; // 读取电机目标位置
        case S_SPOS:
            cmd[i] = 0x34;
            ++i;
            break; // 读取电机实时设定的目标位置
        case S_VEL:
            cmd[i] = 0x35;
            ++i;
            break; // 读取电机实时转速
        case S_CPOS:
            cmd[i] = 0x36;
            ++i;
            break; // 读取电机实时位置
        case S_PERR:
            cmd[i] = 0x37;
            ++i;
            break; // 读取电机位置误差
        case S_VBAT:
            cmd[i] = 0x38;
            ++i;
            break; // 读取多圈编码器电池电压（Y42）
        case S_TEMP:
            cmd[i] = 0x39;
            ++i;
            break; // 读取电机实时温度（X42S/Y42）
        case S_FLAG:
            cmd[i] = 0x3A;
            ++i;
            break; // 读取电机状态标志位
        case S_OFLAG:
            cmd[i] = 0x3B;
            ++i;
            break; // 读取回零状态标志位
        case S_OAF:
            cmd[i] = 0x3C;
            ++i;
            break; // 读取电机状态标志位 + 回零状态标志位（X42S/Y42）
        case S_PIN:
            cmd[i] = 0x3D;
            ++i;
            break; // 读取引脚状态（X42S/Y42）
        case S_SYS:
            cmd[i] = 0x43;
            ++i;
            cmd[i] = 0x7A;
            ++i;
            break; // 读取系统状态参数
        default:
            break;
    }

    cmd[i] = 0x6B;
    ++i; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, i);
}

/* ******************** 读取系统参数命令 */

/*





*/

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

/* 读写驱动参数命令 ******************** */

/******************************************************************
 * \brief      修改电机ID地址
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  id 默认电机ID为1，可修改为1-255，0为广播地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Motor_ID(srl_e idx, u8 addr, bool svF, u8 id)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xAE; // 功能码
    cmd[2] = 0x4B; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = id;   // 默认电机ID为1，可修改为1-255，0为广播地址
    cmd[5] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改细分值
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  mstep 默认细分为16，可修改为1-2556，0为256细分
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_MicroStep(srl_e idx, u8 addr, bool svF, u8 mstep)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;  // 地址
    cmd[1] = 0x84;  // 功能码
    cmd[2] = 0x8A;  // 辅助码
    cmd[3] = svF;   // 是否存储标志，false为不存储，true为存储
    cmd[4] = mstep; // 默认细分为16，可修改为1-2556，0为256细分
    cmd[5] = 0x6B;  // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改掉电标志
 * \param[in]  addr 电机地址
 * \param[in]  pdf 掉电标志
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_PDFlag(srl_e idx, u8 addr, bool pdf)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x50; // 功能码
    cmd[2] = pdf;  // 掉电标志
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      读取选项参数状态（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Opt_Param_Sta(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x1A; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改电机类型（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  mottype 电机类型，默认为0，0表示1.8°步进电机，1表示0.9°步进电机
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Motor_Type(srl_e idx, u8 addr, bool svF, bool mottype)
{
    __IO static u8 cmd[16] = {0};
    u8 MotType             = 0;

    if (mottype) {
        MotType = 25;
    } else {
        MotType = 50;
    }

    // 装载命令
    cmd[0] = addr;    // 地址
    cmd[1] = 0xD7;    // 功能码
    cmd[2] = 0x35;    // 辅助码
    cmd[3] = svF;     // 是否存储标志，false为不存储，true为存储
    cmd[4] = MotType; // 电机类型，0表示0.9°步进电机，1表示1.8°步进电机
    cmd[5] = 0x6B;    // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改固件类型（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  fwtype 固件类型，默认为0，0为X固件，1为Emm固件
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Firmware_Type(srl_e idx, u8 addr, bool svF, bool fwtype)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;   // 地址
    cmd[1] = 0xD5;   // 功能码
    cmd[2] = 0x69;   // 辅助码
    cmd[3] = svF;    // 是否存储标志，false为不存储，true为存储
    cmd[4] = fwtype; // 电机类型，25表示0.9°步进电机，50表示1.8°步进电机
    cmd[5] = 0x6B;   // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改开环/闭环控制模式（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  ctrl_mode 控制模式，默认为1,0为开环模式，1为闭环FOC模式
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Ctrl_Mode(srl_e idx, u8 addr, bool svF, bool ctrl_mode)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;      // 地址
    cmd[1] = 0x46;      // 功能码
    cmd[2] = 0x69;      // 辅助码
    cmd[3] = svF;       // 是否存储标志，false为不存储，true为存储
    cmd[4] = ctrl_mode; // 控制模式，默认为1,0为开环模式，1为闭环FOC模式
    cmd[5] = 0x6B;      // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改电机运动正方向（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  dir 电机运动正方向，默认为CW，0为CW（顺时针方向），1为CCW
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Motor_Dir(srl_e idx, u8 addr, bool svF, bool dir)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xD4; // 功能码
    cmd[2] = 0x60; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = dir;  // 电机运动正方向，默认为CW，0为CW（顺时针方向），1为CCW
    cmd[5] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改锁定按键功能（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  lock 锁定按键功能，默认为Disable，0为Disable，1为Enable
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Lock_Btn(srl_e idx, u8 addr, bool svF, bool lock)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xD0; // 功能码
    cmd[2] = 0xB3; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = lock; // 锁定按键功能，默认为Disable，0为Disable），1为Enable
    cmd[5] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改命令位置角度是否继续缩小10倍输入（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  s_vel 命令位置角度是否继续缩小10倍输入，默认为Disable，0为Disable，1为Enable
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_S_Vel(srl_e idx, u8 addr, bool svF, bool s_vel)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;  // 地址
    cmd[1] = 0x4F;  // 功能码
    cmd[2] = 0x71;  // 辅助码
    cmd[3] = svF;   // 是否存储标志，false为不存储，true为存储
    cmd[4] = s_vel; // 命令位置角度是否继续缩小10倍输入，默认为Disable，0为Disable，1为Enable
    cmd[5] = 0x6B;  // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 6);
}

/******************************************************************
 * \brief      修改开环模式工作电流
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  om_ma 开环模式工作电流，单位mA
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_OM_mA(srl_e idx, u8 addr, bool svF, u16 om_ma)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;             // 地址
    cmd[1] = 0x44;             // 功能码
    cmd[2] = 0x33;             // 辅助码
    cmd[3] = svF;              // 是否存储标志，false为不存储，true为存储
    cmd[4] = (u8)(om_ma >> 8); // 开环模式工作电流，单位mA
    cmd[5] = (u8)(om_ma >> 0);
    cmd[6] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 7);
}

/******************************************************************
 * \brief      修改闭环模式最大电流
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  foc_mA 闭环模式最大电流，单位mA
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_FOC_mA(srl_e idx, u8 addr, bool svF, u16 foc_mA)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;              // 地址
    cmd[1] = 0x45;              // 功能码
    cmd[2] = 0x66;              // 辅助码
    cmd[3] = svF;               // 是否存储标志，false为不存储，true为存储
    cmd[4] = (u8)(foc_mA >> 8); // 闭环模式最大电流，单位mA
    cmd[5] = (u8)(foc_mA >> 0);
    cmd[6] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 7);
}

/******************************************************************
 * \brief      读取PID参数
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_PID_Params(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x21; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改PID参数
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  pTkp 梯形曲线位置环比例系数，默认为126640
 * \param[in]  pBkp 直通限速位置环比例系数，默认为126640
 * \param[in]  vkp 速度环比例系数，42默认为15600
 * \param[in]  vki 速度环积分系数，42默认为26
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_PID_Params(srl_e idx, u8 addr, bool svF, u32 pTkp, u32 pBkp, u32 vkp, u32 vki)
{
    __IO static u8 cmd[32] = {0};

    // 装载命令
    cmd[0]  = addr;             // 地址
    cmd[1]  = 0x4A;             // 功能码
    cmd[2]  = 0xC3;             // 辅助码
    cmd[3]  = svF;              // 是否存储标志，false为不存储，true为存储
    cmd[4]  = (u8)(pTkp >> 24); // pTkp
    cmd[5]  = (u8)(pTkp >> 16);
    cmd[6]  = (u8)(pTkp >> 8);
    cmd[7]  = (u8)(pTkp >> 0);
    cmd[8]  = (u8)(pBkp >> 24); // pBkp
    cmd[9]  = (u8)(pBkp >> 16);
    cmd[10] = (u8)(pBkp >> 8);
    cmd[11] = (u8)(pBkp >> 0);
    cmd[12] = (u8)(vkp >> 24); // vkp
    cmd[13] = (u8)(vkp >> 16);
    cmd[14] = (u8)(vkp >> 8);
    cmd[15] = (u8)(vkp >> 0);
    cmd[16] = (u8)(vki >> 24); // vki
    cmd[17] = (u8)(vki >> 16);
    cmd[18] = (u8)(vki >> 8);
    cmd[19] = (u8)(vki >> 0);
    cmd[20] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 21);
}

/******************************************************************
 * \brief      读取DMX512协议参数（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_DMX512_Params(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x49; // 功能码
    cmd[2] = 0x78; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      读取DMX512协议参数（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF  是否存储标志，false为不存储，true为存储
 * \param[in]  tch 总通道数，默认为192，该值要与自身 DMX512 控制器的总通道数一样
 * \param[in]  nch 每个电机占用的通道数，默认为1，1为单通道模式,2为双通道模式
 * \param[in]  mode 运动模式，默认为1，0表示相对位置模式运动，1表示绝对坐标式位置运动
 * \param[in]  vel 单通道模式的运动速度，默认值为1000， 单位RPM， 即1000RPM；
 * \param[in]  acc 加速度，acc=加速数值/8=125，加速时间见说明书“5.3.12 位置模式控制（Emm）”
 * \param[in]  vel_step 双通道模式速度步长，默认值为 10， 即电机运动速度为(通道值 * 10)RPM
 * \param[in]  pos_step 双通道模式运动步长，默认值为 100， 即电机转动角度为(通道值 * 10.0)°
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_DMX512_Params(srl_e idx, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, u32 pos_step)
{
    __IO static u8 cmd[32] = {0};

    // 装载命令
    cmd[0]  = addr;           // 地址
    cmd[1]  = 0xD9;           // 功能码
    cmd[2]  = 0x90;           // 辅助码
    cmd[3]  = svF;            // 是否存储标志，false为不存储，true为存储
    cmd[4]  = (u8)(tch >> 8); // 总通道数
    cmd[5]  = (u8)(tch >> 0);
    cmd[6]  = nch;            // 每个电机占用的通道数
    cmd[7]  = mode;           // 运动模式
    cmd[8]  = (u8)(vel >> 8); // 单通道模式的运动速度
    cmd[9]  = (u8)(vel >> 0);
    cmd[10] = (u8)(acc >> 8); // 双通道模式速度步长
    cmd[11] = (u8)(acc >> 0);
    cmd[12] = (u8)(vel_step >> 8); // 双通道模式速度步长
    cmd[13] = (u8)(vel_step >> 0);
    cmd[14] = (u8)(pos_step >> 24); // 双通道模式运动步长
    cmd[15] = (u8)(pos_step >> 16);
    cmd[16] = (u8)(pos_step >> 8);
    cmd[17] = (u8)(pos_step >> 0);
    cmd[18] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 19);
}

/******************************************************************
 * \brief      读取位置到达窗口（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Pos_Window(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x41; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改位置到达窗口（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  prw 位置到达窗口，默认值为8，即0.8°
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Pos_Window(srl_e idx, u8 addr, bool svF, u16 prw)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0xD1;           // 功能码
    cmd[2] = 0x07;           // 辅助码
    cmd[3] = svF;            // 是否存储标志，false为不存储，true为存储
    cmd[4] = (u8)(prw >> 8); // 位置到达窗口，默认值为8，即0.8°
    cmd[5] = (u8)(prw >> 0);
    cmd[6] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 7);
}

/******************************************************************
 * \brief      读取过热过流保护检测阈值（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Otocp(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x13; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改过热过流保护检测阈值（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  otp 过热保护检测阈值，默认100℃
 * \param[in]  ocp 过流保护检测阈值，默认6600mA
 * \param[in]  time_ms 过热过流检测时间，默认1000ms
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Otocp(srl_e idx, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0]  = addr;           // 地址
    cmd[1]  = 0xD3;           // 功能码
    cmd[2]  = 0x56;           // 辅助码
    cmd[3]  = svF;            // 是否存储标志，false为不存储，true为存储
    cmd[4]  = (u8)(otp >> 8); // 过热保护检测阈值
    cmd[5]  = (u8)(otp >> 0);
    cmd[6]  = (u8)(ocp >> 8); // 过流保护检测阈值
    cmd[7]  = (u8)(ocp >> 0);
    cmd[8]  = (u8)(time_ms >> 8); // 过热过流检测时间
    cmd[9]  = (u8)(time_ms >> 0);
    cmd[10] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 11);
}

/******************************************************************
 * \brief      读取心跳保护功能时间（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Heart_Protect(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x16; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改心跳保护功能时间（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  hp心跳保护时间，单位：ms
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Heart_Protect(srl_e idx, u8 addr, bool svF, u32 hp)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0x68;           // 功能码
    cmd[2] = 0x38;           // 辅助码
    cmd[3] = svF;            // 是否存储标志，false为不存储，true为存储
    cmd[4] = (u8)(hp >> 24); // 心跳保护时间，单位：ms
    cmd[5] = (u8)(hp >> 16);
    cmd[6] = (u8)(hp >> 8);
    cmd[7] = (u8)(hp >> 0);
    cmd[8] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 9);
}

/******************************************************************
 * \brief      读取积分限幅/刚性系数（X42S/Y42）
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Integral_Limit(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x23; // 功能码
    cmd[2] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 3);
}

/******************************************************************
 * \brief      修改积分限幅/刚性系数（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  il 刚性系数，X 固件默认为X42S/Y42/388、X57S/Y57/512
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Modify_Integral_Limit(srl_e idx, u8 addr, bool svF, u32 il)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0x4B;           // 功能码
    cmd[2] = 0x57;           // 辅助码
    cmd[3] = svF;            // 是否存储标志，false为不存储，true为存储
    cmd[4] = (u8)(il >> 24); // 刚性系数，X 固件默认为X42S/Y42/388、X57S/Y57/512
    cmd[5] = (u8)(il >> 16);
    cmd[6] = (u8)(il >> 8);
    cmd[7] = (u8)(il >> 0);
    cmd[8] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 9);
}

/* ******************** 读写驱动参数命令 */

/*





*/

/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/

/* 读取所有驱动参数命令 ******************** */

/******************************************************************
 * \brief      读取系统状态参数
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_System_State_Params(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x43; // 功能码
    cmd[2] = 0x7A; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/******************************************************************
 * \brief      读取驱动配置参数
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_Read_Motor_Conf_Params(srl_e idx, u8 addr)
{
    __IO static u8 cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x42; // 功能码
    cmd[2] = 0x6C; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 发送命令
    serial_send_x_v2_cmd(idx, cmd, 4);
}

/* ******************** 读写驱动参数命令 */

/*





*/

/**
***********************************************************
***********************************************************
***
***
*** \brief	以下是把相应命令加载到X42S/Y42多电机命令上的函数（X42S/Y42）
***
***
***********************************************************
***********************************************************
***/

/*





*/

/**********************************************************
*** 触发动作命令
**********************************************************/

/* 触发动作命令 ******************** */

/******************************************************************
 * \brief      触发编码器校准 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Trig_Encoder_Cal(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x06; // 功能码
    cmd[2] = 0x45; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      重启电机（X42S/Y42） - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Reset_Motor(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x08; // 功能码
    cmd[2] = 0x97; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      将当前位置清零 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Reset_CurPos_To_Zero(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0A; // 功能码
    cmd[2] = 0x6D; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      解除堵转保护 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Reset_Clog_Pro(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0E; // 功能码
    cmd[2] = 0x52; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      恢复出厂设置 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Restore_Motor(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x0F; // 功能码
    cmd[2] = 0x5F; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 触发动作命令 */

/*





*/

/**********************************************************
*** 运动控制命令
**********************************************************/

/* 运动控制命令 ******************** */

/******************************************************************
 * \brief      使能信号控制 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  state 使能状态，true为使能电机，false为关闭电机
 * \param[in]  snF 多机同步标志 ，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_En_Control(u8 addr, bool state, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;      // 地址
    cmd[1] = 0xF3;      // 功能码
    cmd[2] = 0xAB;      // 辅助码
    cmd[3] = (u8)state; // 使能状态
    cmd[4] = snF;       // 多机同步运动标志
    cmd[5] = 0x6B;      // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 6; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      速度模式 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，其余值为CCW
 * \param[in]  vel 速度，范围0 - 5000RPM
 * \param[in]  acc 加速度，范围0 - 255，注意：0是直接启动
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Vel_Control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;           // 地址
    cmd[1] = 0xF6;           // 功能码
    cmd[2] = dir;            // 方向
    cmd[3] = (u8)(vel >> 8); // 速度(RPM)高8位字节
    cmd[4] = (u8)(vel >> 0); // 速度(RPM)低8位字节
    cmd[5] = acc;            // 加速度，注意：0是直接启动
    cmd[6] = snF;            // 多机同步运动标志
    cmd[7] = 0x6B;           // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 8; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      位置模式 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  dir 方向，0为CW，其余值为CCW
 * \param[in]  vel 速度(RPM)，范围0 - 5000RPM
 * \param[in]  acc 加速度，范围0 - 255，注意：0是直接启动
 * \param[in]  clk 脉冲数，范围0- (2^32 - 1)个
 * \param[in]  raF 相位/绝对标志，false为相对运动，true为绝对值运动
 * \param[in]  snF 多机同步标志 ，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Pos_Control(u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0]  = addr;            // 地址
    cmd[1]  = 0xFD;            // 功能码
    cmd[2]  = dir;             // 方向
    cmd[3]  = (u8)(vel >> 8);  // 速度(RPM)高8位字节
    cmd[4]  = (u8)(vel >> 0);  // 速度(RPM)低8位字节
    cmd[5]  = acc;             // 加速度，注意：0是直接启动
    cmd[6]  = (u8)(clk >> 24); // 脉冲数(bit24 - bit31)
    cmd[7]  = (u8)(clk >> 16); // 脉冲数(bit16 - bit23)
    cmd[8]  = (u8)(clk >> 8);  // 脉冲数(bit8  - bit15)
    cmd[9]  = (u8)(clk >> 0);  // 脉冲数(bit0  - bit7 )
    cmd[10] = raF;             // 相位/绝对标志，false为相对运动，true为绝对值运动
    cmd[11] = snF;             // 多机同步运动标志，false为不启用，true为启用
    cmd[12] = 0x6B;            // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 13; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      立即停止 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Stop_Now(u8 addr, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFE; // 功能码
    cmd[2] = 0x98; // 辅助码
    cmd[3] = snF;  // 多机同步运动标志
    cmd[4] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      多机同步运动 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Synchronous_motion(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0xFF; // 功能码
    cmd[2] = 0x66; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 运动控制命令 */

/*





*/

/**********************************************************
*** 原点回零命令
**********************************************************/

/* 原点回零命令 ******************** */

/******************************************************************
 * \brief      设置单圈回零的零点位置 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Origin_Set_O(u8 addr, bool svF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x93; // 功能码
    cmd[2] = 0x88; // 辅助码
    cmd[3] = svF;  // 是否存储标志，false为不存储，true为存储
    cmd[4] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      触发回零 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  o_mode 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * \param[in]  snF 多机同步标志，false为不启用，true为启用
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Origin_Trigger_Return(u8 addr, u8 o_mode, bool snF)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr;   // 地址
    cmd[1] = 0x9A;   // 功能码
    cmd[2] = o_mode; // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[3] = snF;    // 多机同步运动标志，false为不启用，true为启用
    cmd[4] = 0x6B;   // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 5; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      强制中断并退出回零 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Origin_Interrupt(u8 addr)
{
    u8 j = 0, cmd[16] = {0};

    // 装载命令
    cmd[0] = addr; // 地址
    cmd[1] = 0x9C; // 功能码
    cmd[2] = 0x48; // 辅助码
    cmd[3] = 0x6B; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 4; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      修改回零参数 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  svF 是否存储标志，false为不存储，true为存储
 * \param[in]  o_mode 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
 * \param[in]  o_dir 回零方向，0为CW，其余值为CCW
 * \param[in]  o_vel 回零速度，单位：RPM（转/分钟）
 * \param[in]  o_tm 回零超时时间，单位：毫秒
 * \param[in]  sl_vel 无限位碰撞回零检测转速，单位：RPM（转/分钟）
 * \param[in]  sl_ma 无限位碰撞回零检测电流，单位：Ma（毫安）
 * \param[in]  sl_ms 无限位碰撞回零检测时间，单位：Ms（毫秒）
 * \param[in]  potF 上电自动触发回零，false为不使能，true为使能
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Origin_Modify_Params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
    u8 j = 0, cmd[32] = {0};

    // 装载命令
    cmd[0]  = addr;              // 地址
    cmd[1]  = 0x4C;              // 功能码
    cmd[2]  = 0xAE;              // 辅助码
    cmd[3]  = svF;               // 是否存储标志，false为不存储，true为存储
    cmd[4]  = o_mode;            // 回零模式，0为单圈就近回零，1为单圈方向回零，2为多圈无限位碰撞回零，3为多圈有限位开关回零
    cmd[5]  = o_dir;             // 回零方向
    cmd[6]  = (u8)(o_vel >> 8);  // 回零速度(RPM)高8位字节
    cmd[7]  = (u8)(o_vel >> 0);  // 回零速度(RPM)低8位字节
    cmd[8]  = (u8)(o_tm >> 24);  // 回零超时时间(bit24 - bit31)
    cmd[9]  = (u8)(o_tm >> 16);  // 回零超时时间(bit16 - bit23)
    cmd[10] = (u8)(o_tm >> 8);   // 回零超时时间(bit8  - bit15)
    cmd[11] = (u8)(o_tm >> 0);   // 回零超时时间(bit0  - bit7 )
    cmd[12] = (u8)(sl_vel >> 8); // 无限位碰撞回零检测转速(RPM)高8位字节
    cmd[13] = (u8)(sl_vel >> 0); // 无限位碰撞回零检测转速(RPM)低8位字节
    cmd[14] = (u8)(sl_ma >> 8);  // 无限位碰撞回零检测电流(Ma)高8位字节
    cmd[15] = (u8)(sl_ma >> 0);  // 无限位碰撞回零检测电流(Ma)低8位字节
    cmd[16] = (u8)(sl_ms >> 8);  // 无限位碰撞回零检测时间(Ms)高8位字节
    cmd[17] = (u8)(sl_ms >> 0);  // 无限位碰撞回零检测时间(Ms)低8位字节
    cmd[18] = potF;              // 上电自动触发回零，false为不使能，true为使能
    cmd[19] = 0x6B;              // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < 20; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 原点回零命令 */

/*





*/

/**********************************************************
*** 读取系统参数命令
**********************************************************/

/* 读取系统参数命令 ******************** */

/******************************************************************
 * \brief      定时返回信息命令（X42S/Y42）
 * \param[in]  addr 电机地址
 * \param[in]  s 系统参数类型
 * \param[in]  time_ms 定时时间
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Auto_Return_Sys_Params_Timed(u8 addr, zdtSysParams_e s, u16 time_ms)
{
    u8 i = 0, j = 0;
    u8 cmd[16] = {0};

    // 装载命令
    cmd[i] = addr;
    ++i; // 地址

    cmd[i] = 0x11;
    ++i; // 功能码

    cmd[i] = 0x18;
    ++i; // 辅助码

    switch (s) // 信息功能码
    {
        case S_VBUS:
            cmd[i] = 0x24;
            ++i;
            break; // 读取总线电压
        case S_CBUS:
            cmd[i] = 0x26;
            ++i;
            break; // 读取总线电流
        case S_CPHA:
            cmd[i] = 0x27;
            ++i;
            break; // 读取相电流
        case S_ENCO:
            cmd[i] = 0x29;
            ++i;
            break; // 读取编码器原始值
        case S_CLKC:
            cmd[i] = 0x30;
            ++i;
            break; // 读取实时脉冲数
        case S_ENCL:
            cmd[i] = 0x31;
            ++i;
            break; // 读取经过线性化校准后的编码器值
        case S_CLKI:
            cmd[i] = 0x32;
            ++i;
            break; // 读取输入脉冲数
        case S_TPOS:
            cmd[i] = 0x33;
            ++i;
            break; // 读取电机目标位置
        case S_SPOS:
            cmd[i] = 0x34;
            ++i;
            break; // 读取电机实时设定的目标位置
        case S_VEL:
            cmd[i] = 0x35;
            ++i;
            break; // 读取电机实时转速
        case S_CPOS:
            cmd[i] = 0x36;
            ++i;
            break; // 读取电机实时位置
        case S_PERR:
            cmd[i] = 0x37;
            ++i;
            break; // 读取电机位置误差
        case S_VBAT:
            cmd[i] = 0x38;
            ++i;
            break; // 读取多圈编码器电池电压（Y42）
        case S_TEMP:
            cmd[i] = 0x39;
            ++i;
            break; // 读取电机实时温度（X42S/Y42）
        case S_FLAG:
            cmd[i] = 0x3A;
            ++i;
            break; // 读取电机状态标志位
        case S_OFLAG:
            cmd[i] = 0x3B;
            ++i;
            break; // 读取回零状态标志位
        case S_OAF:
            cmd[i] = 0x3C;
            ++i;
            break; // 读取电机状态标志位 + 回零状态标志位（X42S/Y42）
        case S_PIN:
            cmd[i] = 0x3D;
            ++i;
            break; // 读取引脚状态（X42S/Y42）
        default:
            break;
    }

    cmd[i] = (u8)(time_ms >> 8);
    ++i; // 定时时间
    cmd[i] = (u8)(time_ms >> 0);
    ++i;

    cmd[i] = 0x6B;
    ++i; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < i; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/******************************************************************
 * \brief      读取系统参数 - 加载到多电机指令上
 * \param[in]  addr 电机地址
 * \param[in]  s 系统参数类型
 * \retval     地址 + 功能码 + 命令状态 + 校验字节
 */
void X_V2_MMCL_Read_Sys_Params(u8 addr, zdtSysParams_e s)
{
    u8 i = 0, j = 0;
    u8 cmd[16] = {0};

    // 装载命令
    cmd[i] = addr;
    ++i; // 地址

    switch (s) // 功能码
    {
        case S_VBUS:
            cmd[i] = 0x24;
            ++i;
            break; // 读取总线电压
        case S_CBUS:
            cmd[i] = 0x26;
            ++i;
            break; // 读取总线电流
        case S_CPHA:
            cmd[i] = 0x27;
            ++i;
            break; // 读取相电流
        case S_ENCO:
            cmd[i] = 0x29;
            ++i;
            break; // 读取编码器原始值
        case S_CLKC:
            cmd[i] = 0x30;
            ++i;
            break; // 读取实时脉冲数
        case S_ENCL:
            cmd[i] = 0x31;
            ++i;
            break; // 读取经过线性化校准后的编码器值
        case S_CLKI:
            cmd[i] = 0x32;
            ++i;
            break; // 读取输入脉冲数
        case S_TPOS:
            cmd[i] = 0x33;
            ++i;
            break; // 读取电机目标位置
        case S_SPOS:
            cmd[i] = 0x34;
            ++i;
            break; // 读取电机实时设定的目标位置
        case S_VEL:
            cmd[i] = 0x35;
            ++i;
            break; // 读取电机实时转速
        case S_CPOS:
            cmd[i] = 0x36;
            ++i;
            break; // 读取电机实时位置
        case S_PERR:
            cmd[i] = 0x37;
            ++i;
            break; // 读取电机位置误差
        case S_VBAT:
            cmd[i] = 0x38;
            ++i;
            break; // 读取多圈编码器电池电压（Y42）
        case S_TEMP:
            cmd[i] = 0x39;
            ++i;
            break; // 读取电机实时温度（X42S/Y42）
        case S_FLAG:
            cmd[i] = 0x3A;
            ++i;
            break; // 读取电机状态标志位
        case S_OFLAG:
            cmd[i] = 0x3B;
            ++i;
            break; // 读取回零状态标志位
        case S_OAF:
            cmd[i] = 0x3C;
            ++i;
            break; // 读取电机状态标志位 + 回零状态标志位（X42S/Y42）
        case S_PIN:
            cmd[i] = 0x3D;
            ++i;
            break; // 读取引脚状态（X42S/Y42）
        case S_SYS:
            cmd[i] = 0x43;
            ++i;
            cmd[i] = 0x7A;
            ++i;
            break; // 读取系统状态参数
        default:
            break;
    }

    cmd[i] = 0x6B;
    ++i; // 校验字节

    // 加载当前命令到多电机命令中
    for (j = 0; j < i; j++) {
        MMCL_cmd[MMCL_count] = cmd[j];
        ++MMCL_count;
    }
}

/* ******************** 读取系统参数命令 */

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

#endif
