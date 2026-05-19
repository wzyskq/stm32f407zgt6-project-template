/******************************************************************
 ** \file    zdt_api.c
 **
 ** \author  Yiiry
 **
 ** \brief  本文件（zdt_api.c/.h）主要功能为发送电机控制命令.
 **         衍生文件（zdt_pro.c/.h）处理电机返回数据包的接收、解析.
 **
 ** \pre    需配合 Emm_V5.c/.h 和 X_V2.c/.h 使用
 **
 ** \note   张大头闭环步进电机本身就是集成好的闭环系统，一般情况下直接用本文件提供的命令控制电机即可.
 */

#include "zdt_api.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/**
 * \brief    触发编码器校准
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_trig_encoder_cal(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Trig_Encoder_Cal(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Trig_Encoder_Cal(idx, addr);
#endif
}

/**
 * \brief    重启电机
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_reset_motor(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Reset_Motor(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Reset_Motor(idx, addr);
#endif
}

/**
 * \brief    将当前位置清零
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_reset_curpos_to_zero(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Reset_CurPos_To_Zero(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Reset_CurPos_To_Zero(idx, addr);
#endif
}

/**
 * \brief    解除堵转保护
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_reset_clog_pro(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Reset_Clog_Pro(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Reset_Clog_Pro(idx, addr);
#endif
}

/**
 * \brief    恢复出厂设置
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_restore_motor(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Restore_Motor(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Restore_Motor(idx, addr);
#endif
}

/**
 * \brief    多电机命令触发
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_multi_motor_cmd(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Multi_Motor_Cmd(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Multi_Motor_Cmd(idx, addr);
#endif
}

/**
 * \brief    电机使能控制
 * \param[in] idx   串口索引
 * \param[in] addr  电机地址
 * \param[in] state 使能状态
 * \param[in] snF   多机同步标志
 */
void zdt_en_control(srl_e idx, u8 addr, bool state, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_En_Control(idx, addr, state, snF);
#elif defined(ZDT_X_V2)
    X_V2_En_Control(idx, addr, state, snF);
#endif
}

/**
 * \brief    速度模式控制
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] dir  方向
 * \param[in] vel  速度
 * \param[in] acc  加速度
 * \param[in] snF  多机同步标志
 */
void zdt_vel_control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Vel_Control(idx, addr, dir, vel, acc, snF);
#elif defined(ZDT_X_V2)
    X_V2_Vel_Control(idx, addr, dir, acc, (float)vel, snF);
#endif
}

/**
 * \brief    位置模式控制
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] dir  方向
 * \param[in] vel  速度
 * \param[in] acc  加速度
 * \param[in] clk  脉冲数
 * \param[in] raF  相对/绝对标志
 * \param[in] snF  多机同步标志
 */
void zdt_pos_control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Pos_Control(idx, addr, dir, vel, acc, clk, raF, snF);
#elif defined(ZDT_X_V2)
    X_V2_Bypass_Pos_LV_Control(idx, addr, dir, (float)vel, (float)clk, raF, snF);
#endif
}

/**
 * \brief    立即停止
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] snF  多机同步标志
 */
void zdt_stop_now(srl_e idx, u8 addr, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Stop_Now(idx, addr, snF);
#elif defined(ZDT_X_V2)
    X_V2_Stop_Now(idx, addr, snF);
#endif
}

/**
 * \brief    多机同步运动触发
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_synchronous_motion(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Synchronous_motion(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Synchronous_motion(idx, addr);
#endif
}

/**
 * \brief    设置回零零点位置
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 */
void zdt_origin_set_o(srl_e idx, u8 addr, bool svF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Origin_Set_O(idx, addr, svF);
#elif defined(ZDT_X_V2)
    X_V2_Origin_Set_O(idx, addr, svF);
#endif
}

/**
 * \brief    触发回零
 * \param[in] idx    串口索引
 * \param[in] addr   电机地址
 * \param[in] o_mode 回零模式
 * \param[in] snF    多机同步标志
 */
void zdt_origin_trigger_return(srl_e idx, u8 addr, u8 o_mode, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Origin_Trigger_Return(idx, addr, o_mode, snF);
#elif defined(ZDT_X_V2)
    X_V2_Origin_Trigger_Return(idx, addr, o_mode, snF);
#endif
}

/**
 * \brief    强制中断回零
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_origin_interrupt(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Origin_Interrupt(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Origin_Interrupt(idx, addr);
#endif
}

/**
 * \brief    读取回零参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_origin_read_params(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Origin_Read_Params(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Origin_Read_Params(idx, addr);
#endif
}

/**
 * \brief    修改回零参数
 * \param[in] idx    串口索引
 * \param[in] addr   电机地址
 * \param[in] svF    是否存储
 * \param[in] o_mode 回零模式
 * \param[in] o_dir  回零方向
 * \param[in] o_vel  回零速度
 * \param[in] o_tm   回零超时
 * \param[in] sl_vel 碰撞检测转速
 * \param[in] sl_ma  碰撞检测电流
 * \param[in] sl_ms  碰撞检测时间
 * \param[in] potF   上电自动回零标志
 */
void zdt_origin_modify_params(srl_e idx, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Origin_Modify_Params(idx, addr, svF, o_mode, o_dir, o_vel, o_tm, sl_vel, sl_ma, sl_ms, potF);
#elif defined(ZDT_X_V2)
    X_V2_Origin_Modify_Params(idx, addr, svF, o_mode, o_dir, o_vel, o_tm, sl_vel, sl_ma, sl_ms, potF);
#endif
}

/**
 * \brief    定时返回系统参数
 * \param[in] idx     串口索引
 * \param[in] addr    电机地址
 * \param[in] s       系统参数类型
 * \param[in] time_ms 定时时间
 */
void zdt_auto_return_sys_params_timed(srl_e idx, u8 addr, zdtSysParams_e s, u16 time_ms)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Auto_Return_Sys_Params_Timed(idx, addr, s, time_ms);
#elif defined(ZDT_X_V2)
    X_V2_Auto_Return_Sys_Params_Timed(idx, addr, s, time_ms);
#endif
}

/**
 * \brief    读取系统参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] s    系统参数类型
 */
void zdt_read_sys_params(srl_e idx, u8 addr, zdtSysParams_e s)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Sys_Params(idx, addr, s);
#elif defined(ZDT_X_V2)
    X_V2_Read_Sys_Params(idx, addr, s);
#endif
}

/**
 * \brief    修改电机 ID
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] id   新 ID
 */
void zdt_modify_motor_id(srl_e idx, u8 addr, bool svF, u8 id)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Motor_ID(idx, addr, svF, id);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Motor_ID(idx, addr, svF, id);
#endif
}

/**
 * \brief    修改细分值
 * \param[in] idx   串口索引
 * \param[in] addr  电机地址
 * \param[in] svF   是否存储
 * \param[in] mstep 细分值
 */
void zdt_modify_micro_step(srl_e idx, u8 addr, bool svF, u8 mstep)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_MicroStep(idx, addr, svF, mstep);
#elif defined(ZDT_X_V2)
    X_V2_Modify_MicroStep(idx, addr, svF, mstep);
#endif
}

/**
 * \brief    修改掉电标志
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] pdf  掉电标志
 */
void zdt_modify_pdflag(srl_e idx, u8 addr, bool pdf)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_PDFlag(idx, addr, pdf);
#elif defined(ZDT_X_V2)
    X_V2_Modify_PDFlag(idx, addr, pdf);
#endif
}

/**
 * \brief    读取选项参数状态
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_opt_param_sta(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Opt_Param_Sta(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Opt_Param_Sta(idx, addr);
#endif
}

/**
 * \brief    修改电机类型
 * \param[in] idx     串口索引
 * \param[in] addr    电机地址
 * \param[in] svF     是否存储
 * \param[in] mottype 电机类型
 */
void zdt_modify_motor_type(srl_e idx, u8 addr, bool svF, bool mottype)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Motor_Type(idx, addr, svF, mottype);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Motor_Type(idx, addr, svF, mottype);
#endif
}

/**
 * \brief    修改固件类型
 * \param[in] idx    串口索引
 * \param[in] addr   电机地址
 * \param[in] svF    是否存储
 * \param[in] fwtype 固件类型
 */
void zdt_modify_firmware_type(srl_e idx, u8 addr, bool svF, bool fwtype)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Firmware_Type(idx, addr, svF, fwtype);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Firmware_Type(idx, addr, svF, fwtype);
#endif
}

/**
 * \brief    修改控制模式
 * \param[in] idx       串口索引
 * \param[in] addr      电机地址
 * \param[in] svF       是否存储
 * \param[in] ctrl_mode 控制模式
 */
void zdt_modify_ctrl_mode(srl_e idx, u8 addr, bool svF, bool ctrl_mode)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Ctrl_Mode(idx, addr, svF, ctrl_mode);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Ctrl_Mode(idx, addr, svF, ctrl_mode);
#endif
}

/**
 * \brief    修改电机方向
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] dir  方向
 */
void zdt_modify_motor_dir(srl_e idx, u8 addr, bool svF, bool dir)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Motor_Dir(idx, addr, svF, dir);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Motor_Dir(idx, addr, svF, dir);
#endif
}

/**
 * \brief    修改锁定按键功能
 * \param[in] idx     串口索引
 * \param[in] addr    电机地址
 * \param[in] svF     是否存储
 * \param[in] lockbtn 锁定按键功能
 */
void zdt_modify_lock_btn(srl_e idx, u8 addr, bool svF, bool lockbtn)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Lock_Btn(idx, addr, svF, lockbtn);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Lock_Btn(idx, addr, svF, lockbtn);
#endif
}

/**
 * \brief    修改速度缩放标志
 * \param[in] idx   串口索引
 * \param[in] addr  电机地址
 * \param[in] svF   是否存储
 * \param[in] s_vel 速度缩放标志
 */
void zdt_modify_s_vel(srl_e idx, u8 addr, bool svF, bool s_vel)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_S_Vel(idx, addr, svF, s_vel);
#elif defined(ZDT_X_V2)
    X_V2_Modify_S_Vel(idx, addr, svF, s_vel);
#endif
}

/**
 * \brief    修改开环电流
 * \param[in] idx   串口索引
 * \param[in] addr  电机地址
 * \param[in] svF   是否存储
 * \param[in] om_ma 电流值
 */
void zdt_modify_om_ma(srl_e idx, u8 addr, bool svF, u16 om_ma)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_OM_mA(idx, addr, svF, om_ma);
#elif defined(ZDT_X_V2)
    X_V2_Modify_OM_mA(idx, addr, svF, om_ma);
#endif
}

/**
 * \brief    修改闭环最大电流
 * \param[in] idx    串口索引
 * \param[in] addr   电机地址
 * \param[in] svF    是否存储
 * \param[in] foc_mA 电流值
 */
void zdt_modify_foc_ma(srl_e idx, u8 addr, bool svF, u16 foc_mA)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_FOC_mA(idx, addr, svF, foc_mA);
#elif defined(ZDT_X_V2)
    X_V2_Modify_FOC_mA(idx, addr, svF, foc_mA);
#endif
}

/**
 * \brief    读取 PID 参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_pid_params(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_PID_Params(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_PID_Params(idx, addr);
#endif
}

/**
 * \brief    修改 PID 参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] kp   比例系数
 * \param[in] ki   积分系数
 * \param[in] kd   微分系数
 */
void zdt_modify_pid_params(srl_e idx, u8 addr, bool svF, u32 kp, u32 ki, u32 kd)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_PID_Params(idx, addr, svF, kp, ki, kd);
#elif defined(ZDT_X_V2)
    X_V2_Modify_PID_Params(idx, addr, svF, kp, ki, kd, 0);
#endif
}

/**
 * \brief    读取 DMX512 参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_dmx512_params(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_DMX512_Params(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_DMX512_Params(idx, addr);
#endif
}

/**
 * \brief    修改 DMX512 参数
 * \param[in] idx      串口索引
 * \param[in] addr     电机地址
 * \param[in] svF      是否存储
 * \param[in] tch      总通道数
 * \param[in] nch      每电机通道数
 * \param[in] mode     运动模式
 * \param[in] vel      速度
 * \param[in] acc      加速度
 * \param[in] vel_step 速度步长
 * \param[in] pos_step 位置步长
 */
void zdt_modify_dmx512_params(srl_e idx, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, u32 pos_step)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_DMX512_Params(idx, addr, svF, tch, nch, mode, vel, acc, vel_step, pos_step);
#elif defined(ZDT_X_V2)
    X_V2_Modify_DMX512_Params(idx, addr, svF, tch, nch, mode, vel, acc, vel_step, pos_step);
#endif
}

/**
 * \brief    读取位置到达窗口
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_pos_window(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Pos_Window(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Pos_Window(idx, addr);
#endif
}

/**
 * \brief    修改位置到达窗口
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] prw  窗口值
 */
void zdt_modify_pos_window(srl_e idx, u8 addr, bool svF, u16 prw)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Pos_Window(idx, addr, svF, prw);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Pos_Window(idx, addr, svF, prw);
#endif
}

/**
 * \brief    读取过热过流保护阈值
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_otocp(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Otocp(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Otocp(idx, addr);
#endif
}

/**
 * \brief    修改过热过流保护阈值
 * \param[in] idx     串口索引
 * \param[in] addr    电机地址
 * \param[in] svF     是否存储
 * \param[in] otp     过热阈值
 * \param[in] ocp     过流阈值
 * \param[in] time_ms 检测时间
 */
void zdt_modify_otocp(srl_e idx, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Otocp(idx, addr, svF, otp, ocp, time_ms);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Otocp(idx, addr, svF, otp, ocp, time_ms);
#endif
}

/**
 * \brief    读取心跳保护时间
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_heart_protect(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Heart_Protect(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Heart_Protect(idx, addr);
#endif
}

/**
 * \brief    修改心跳保护时间
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] hp   心跳时间
 */
void zdt_modify_heart_protect(srl_e idx, u8 addr, bool svF, u32 hp)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Heart_Protect(idx, addr, svF, hp);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Heart_Protect(idx, addr, svF, hp);
#endif
}

/**
 * \brief    读取积分限幅/刚性系数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_integral_limit(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Integral_Limit(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Integral_Limit(idx, addr);
#endif
}

/**
 * \brief    修改积分限幅/刚性系数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 * \param[in] il   参数值
 */
void zdt_modify_integral_limit(srl_e idx, u8 addr, bool svF, u32 il)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Modify_Integral_Limit(idx, addr, svF, il);
#elif defined(ZDT_X_V2)
    X_V2_Modify_Integral_Limit(idx, addr, svF, il);
#endif
}

/**
 * \brief    读取系统状态参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_system_state_params(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_System_State_Params(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_System_State_Params(idx, addr);
#endif
}

/**
 * \brief    读取驱动配置参数
 * \param[in] idx  串口索引
 * \param[in] addr 电机地址
 */
void zdt_read_motor_conf_params(srl_e idx, u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_Read_Motor_Conf_Params(idx, addr);
#elif defined(ZDT_X_V2)
    X_V2_Read_Motor_Conf_Params(idx, addr);
#endif
}

/**
 * \brief    多机：触发编码器校准
 * \param[in] addr 电机地址
 */
void zdt_mmcl_trig_encoder_cal(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Trig_Encoder_Cal(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Trig_Encoder_Cal(addr);
#endif
}

/**
 * \brief    多机：重启电机
 * \param[in] addr 电机地址
 */
void zdt_mmcl_reset_motor(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Reset_Motor(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Reset_Motor(addr);
#endif
}

/**
 * \brief    多机：将当前位置清零
 * \param[in] addr 电机地址
 */
void zdt_mmcl_reset_curpos_to_zero(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Reset_CurPos_To_Zero(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Reset_CurPos_To_Zero(addr);
#endif
}

/**
 * \brief    多机：解除堵转保护
 * \param[in] addr 电机地址
 */
void zdt_mmcl_reset_clog_pro(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Reset_Clog_Pro(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Reset_Clog_Pro(addr);
#endif
}

/**
 * \brief    多机：恢复出厂设置
 * \param[in] addr 电机地址
 */
void zdt_mmcl_restore_motor(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Restore_Motor(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Restore_Motor(addr);
#endif
}

/**
 * \brief    多机：电机使能控制
 * \param[in] addr  电机地址
 * \param[in] state 使能状态
 * \param[in] snF   多机同步标志
 */
void zdt_mmcl_en_control(u8 addr, bool state, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_En_Control(addr, state, snF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_En_Control(addr, state, snF);
#endif
}

/**
 * \brief    多机：速度模式控制
 * \param[in] addr 电机地址
 * \param[in] dir  方向
 * \param[in] vel  速度
 * \param[in] acc  加速度
 * \param[in] snF  多机同步标志
 */
void zdt_mmcl_vel_control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Vel_Control(addr, dir, vel, acc, snF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Vel_Control(addr, dir, vel, acc, snF);
#endif
}

/**
 * \brief    多机：位置模式控制
 * \param[in] addr 电机地址
 * \param[in] dir  方向
 * \param[in] vel  速度
 * \param[in] acc  加速度
 * \param[in] clk  脉冲数
 * \param[in] raF  相对/绝对标志
 * \param[in] snF  多机同步标志
 */
void zdt_mmcl_pos_control(u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Pos_Control(addr, dir, vel, acc, clk, raF, snF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Pos_Control(addr, dir, vel, acc, clk, raF, snF);
#endif
}

/**
 * \brief    多机：立即停止
 * \param[in] addr 电机地址
 * \param[in] snF  多机同步标志
 */
void zdt_mmcl_stop_now(u8 addr, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Stop_Now(addr, snF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Stop_Now(addr, snF);
#endif
}

/**
 * \brief    多机：同步运动触发
 * \param[in] addr 电机地址
 */
void zdt_mmcl_synchronous_motion(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Synchronous_motion(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Synchronous_motion(addr);
#endif
}

/**
 * \brief    多机：设置回零零点
 * \param[in] addr 电机地址
 * \param[in] svF  是否存储
 */
void zdt_mmcl_origin_set_o(u8 addr, bool svF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Origin_Set_O(addr, svF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Origin_Set_O(addr, svF);
#endif
}

/**
 * \brief    多机：触发回零
 * \param[in] addr   电机地址
 * \param[in] o_mode 回零模式
 * \param[in] snF    多机同步标志
 */
void zdt_mmcl_origin_trigger_return(u8 addr, u8 o_mode, bool snF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Origin_Trigger_Return(addr, o_mode, snF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Origin_Trigger_Return(addr, o_mode, snF);
#endif
}

/**
 * \brief    多机：中断回零
 * \param[in] addr 电机地址
 */
void zdt_mmcl_origin_interrupt(u8 addr)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Origin_Interrupt(addr);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Origin_Interrupt(addr);
#endif
}

/**
 * \brief    多机：修改回零参数
 * \param[in] addr   电机地址
 * \param[in] svF    是否存储
 * \param[in] o_mode 回零模式
 * \param[in] o_dir  回零方向
 * \param[in] o_vel  回零速度
 * \param[in] o_tm   回零超时
 * \param[in] sl_vel 碰撞检测转速
 * \param[in] sl_ma  碰撞检测电流
 * \param[in] sl_ms  碰撞检测时间
 * \param[in] potF   上电自动回零标志
 */
void zdt_mmcl_origin_modify_params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Origin_Modify_Params(addr, svF, o_mode, o_dir, o_vel, o_tm, sl_vel, sl_ma, sl_ms, potF);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Origin_Modify_Params(addr, svF, o_mode, o_dir, o_vel, o_tm, sl_vel, sl_ma, sl_ms, potF);
#endif
}

/**
 * \brief    多机：定时返回系统参数
 * \param[in] addr    电机地址
 * \param[in] s       系统参数类型
 * \param[in] time_ms 定时时间
 */
void zdt_mmcl_auto_return_sys_params_timed(u8 addr, zdtSysParams_e s, u16 time_ms)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Auto_Return_Sys_Params_Timed(addr, s, time_ms);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Auto_Return_Sys_Params_Timed(addr, s, time_ms);
#endif
}

/**
 * \brief    多机：读取系统参数
 * \param[in] addr 电机地址
 * \param[in] s    系统参数类型
 */
void zdt_mmcl_read_sys_params(u8 addr, zdtSysParams_e s)
{
#if defined(ZDT_EMM_V5)
    Emm_V5_MMCL_Read_Sys_Params(addr, s);
#elif defined(ZDT_X_V2)
    X_V2_MMCL_Read_Sys_Params(addr, s);
#endif
}

