/******************************************************************
 ** \file   X_V2.h
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

#ifndef __X_V2_H
#define __X_V2_H

#include "serial.h"
#include "zdt_api.h"

#if !defined(ZDT_EMM_V5) && defined(ZDT_X_V2) // 仅当定义 ZDT_X_V2 时编译当前文件内容

/**********************************************************
***	X_V2步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

/* Global Macros ----------------------------------------------------------- */

#define MMCL_LEN 512

/* Private Types ----------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

extern __IO u16 MMCL_count, MMCL_cmd[MMCL_LEN];

/* Global Functions -------------------------------------------------------- */

/**
***********************************************************
***********************************************************
***
***
*** @brief	后缀带有（X42S/Y42）为X42S/Y42新增命令，X42不能用，其他通用
***
***
***********************************************************
***********************************************************
***/

/**********************************************************
*** 触发动作命令
**********************************************************/

// 触发编码器校准
// 重启电机（X42S/Y42）
// 将当前位置清零
// 解除堵转保护
// 恢复出厂设置

void X_V2_Trig_Encoder_Cal(srl_e idx, u8 addr);
void X_V2_Reset_Motor(srl_e idx, u8 addr);
void X_V2_Reset_CurPos_To_Zero(srl_e idx, u8 addr);
void X_V2_Reset_Clog_Pro(srl_e idx, u8 addr);
void X_V2_Restore_Motor(srl_e idx, u8 addr);

/**********************************************************
*** 运动控制命令
**********************************************************/

// 多电机命令（X42S/Y42）
// 电机使能控制
// 力矩模式
// 力矩模式限速控制（X42S/Y42）

void X_V2_Multi_Motor_Cmd(srl_e idx, u8 addr);
void X_V2_En_Control(srl_e idx, u8 addr, bool state, bool snF);
void X_V2_Torque_Control(srl_e idx, u8 addr, u8 sign, u16 t_ramp, u16 torque, bool snF);
void X_V2_Torque_LV_Control(srl_e idx, u8 addr, u8 sign, u16 t_ramp, u16 torque, bool snF, float maxVel);

// 速度模式控制
// 速度模式限电流控制（X42S/Y42）
// 直通限速位置模式控制
// 直通限速位置模式限电流控制（X42S/Y42）
// 梯形曲线加减速位置模式控制
// 梯形曲线加减速位置模式限电流控制（X42S/Y42）

void X_V2_Vel_Control(srl_e idx, u8 addr, u8 dir, u16 acc, float vel, bool snF);
void X_V2_Vel_LC_Control(srl_e idx, u8 addr, u8 dir, u16 acc, float vel, bool snF, u16 maxCur);
void X_V2_Bypass_Pos_LV_Control(srl_e idx, u8 addr, u8 dir, float vel, float pos, u8 raf, bool snF);
void X_V2_Bypass_Pos_LV_LC_Control(srl_e idx, u8 addr, u8 dir, float vel, float pos, u8 raf, bool snF, u16 maxCur);
void X_V2_Traj_Pos_Control(srl_e idx, u8 addr, u8 dir, u16 acc, u16 dec, float vel, float pos, u8 raf, bool snF);
void X_V2_Traj_Pos_LC_Control(srl_e idx, u8 addr, u8 dir, u16 acc, u16 dec, float vel, float pos, u8 raf, bool snF, u16 maxCur);

// 让电机立即停止运动
// 触发多机同步开始运动

void X_V2_Stop_Now(srl_e idx, u8 addr, bool snF);
void X_V2_Synchronous_motion(srl_e idx, u8 addr);

/**********************************************************
*** 原点回零命令
**********************************************************/

// 设置单圈回零的零点位置
// 触发回零
// 强制中断并退出回零
// 读取回零参数
// 修改回零参数

void X_V2_Origin_Set_O(srl_e idx, u8 addr, bool svF);
void X_V2_Origin_Trigger_Return(srl_e idx, u8 addr, u8 o_mode, bool snF);
void X_V2_Origin_Interrupt(srl_e idx, u8 addr);
void X_V2_Origin_Read_Params(srl_e idx, u8 addr);
void X_V2_Origin_Modify_Params(srl_e idx, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);

/**********************************************************
*** 读取系统参数命令
**********************************************************/

// 定时返回信息命令（X42S/Y42）
// 读取系统参数

void X_V2_Auto_Return_Sys_Params_Timed(srl_e idx, u8 addr, zdtSysParams_e s, u16 time_ms);
void X_V2_Read_Sys_Params(srl_e idx, u8 addr, zdtSysParams_e s);

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

// 修改电机ID地址
// 修改细分值
// 修改掉电标志
// 读取选项参数状态（X42S/Y42）
// 修改电机类型（X42S/Y42）
// 修改固件类型（X42S/Y42）

void X_V2_Modify_Motor_ID(srl_e idx, u8 addr, bool svF, u8 id);
void X_V2_Modify_MicroStep(srl_e idx, u8 addr, bool svF, u8 mstep);
void X_V2_Modify_PDFlag(srl_e idx, u8 addr, bool pdf);
void X_V2_Read_Opt_Param_Sta(srl_e idx, u8 addr);
void X_V2_Modify_Motor_Type(srl_e idx, u8 addr, bool svF, bool mottype);
void X_V2_Modify_Firmware_Type(srl_e idx, u8 addr, bool svF, bool fwtype);

// 修改开环/闭环控制模式（X42S/Y42）
// 修改电机运动正方向（X42S/Y42）
// 修改锁定按键功能（X42S/Y42）
// 修改命令位置角度是否继续缩小10倍输入（X42S/Y42）
// 修改开环模式工作电流
// 修改闭环模式最大电流

void X_V2_Modify_Ctrl_Mode(srl_e idx, u8 addr, bool svF, bool ctrl_mode);
void X_V2_Modify_Motor_Dir(srl_e idx, u8 addr, bool svF, bool dir);
void X_V2_Modify_Lock_Btn(srl_e idx, u8 addr, bool svF, bool lockbtn);
void X_V2_Modify_S_Vel(srl_e idx, u8 addr, bool svF, bool s_vel);
void X_V2_Modify_OM_mA(srl_e idx, u8 addr, bool svF, u16 om_ma);
void X_V2_Modify_FOC_mA(srl_e idx, u8 addr, bool svF, u16 foc_mA);

// 读取PID参数
// 修改PID参数
// 读取DMX512协议参数（X42S/Y42）
// 修改DMX512协议参数（X42S/Y42）
// 读取位置到达窗口（X42S/Y42）
// 修改位置到达窗口（X42S/Y42）

void X_V2_Read_PID_Params(srl_e idx, u8 addr);
void X_V2_Modify_PID_Params(srl_e idx, u8 addr, bool svF, u32 pTkp, u32 pBkp, u32 vkp, u32 vki);
void X_V2_Read_DMX512_Params(srl_e idx, u8 addr);
void X_V2_Modify_DMX512_Params(srl_e idx, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, u32 pos_step);
void X_V2_Read_Pos_Window(srl_e idx, u8 addr);
void X_V2_Modify_Pos_Window(srl_e idx, u8 addr, bool svF, u16 prw);

// 读取过热过流保护检测阈值（X42S/Y42）
// 修改过热过流保护检测阈值（X42S/Y42）
// 读取心跳保护功能时间（X42S/Y42）
// 修改心跳保护功能时间（X42S/Y42）
// 读取积分限幅/刚性系数（X42S/Y42）
// 修改积分限幅/刚性系数（X42S/Y42）

void X_V2_Read_Otocp(srl_e idx, u8 addr);
void X_V2_Modify_Otocp(srl_e idx, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms);
void X_V2_Read_Heart_Protect(srl_e idx, u8 addr);
void X_V2_Modify_Heart_Protect(srl_e idx, u8 addr, bool svF, u32 hp);
void X_V2_Read_Integral_Limit(srl_e idx, u8 addr);
void X_V2_Modify_Integral_Limit(srl_e idx, u8 addr, bool svF, u32 il);

/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/

// 读取系统状态参数
// 读取驱动配置参数

void X_V2_Read_System_State_Params(srl_e idx, u8 addr);
void X_V2_Read_Motor_Conf_Params(srl_e idx, u8 addr);

/**
***********************************************************
***********************************************************
***
***
*** @brief	以下是把相应命令加载到X42S/Y42多电机命令上的函数（X42S/Y42）
***
***
***********************************************************
***********************************************************
***/

/**********************************************************
*** 触发动作命令
**********************************************************/

// 触发编码器校准 - 加载到多电机指令上
// 重启电机 - 加载到多电机指令上
// 将当前位置清零 - 加载到多电机指令上
// 解除堵转保护 - 加载到多电机指令上
// 恢复出厂设置 - 加载到多电机指令上

void X_V2_MMCL_Trig_Encoder_Cal(u8 addr);
void X_V2_MMCL_Reset_Motor(u8 addr);
void X_V2_MMCL_Reset_CurPos_To_Zero(u8 addr);
void X_V2_MMCL_Reset_Clog_Pro(u8 addr);
void X_V2_MMCL_Restore_Motor(u8 addr);

/**********************************************************
*** 运动控制命令
**********************************************************/

// 电机使能控制 - 加载到多电机指令上
// 速度模式控制 - 加载到多电机指令上
// 位置模式控制 - 加载到多电机指令上
// 让电机立即停止运动 - 加载到多电机指令上
// 触发多机同步开始运动 - 加载到多电机指令上

void X_V2_MMCL_En_Control(u8 addr, bool state, bool snF);
void X_V2_MMCL_Vel_Control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF);
void X_V2_MMCL_Pos_Control(u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF);
void X_V2_MMCL_Stop_Now(u8 addr, bool snF);
void X_V2_MMCL_Synchronous_motion(u8 addr);

/**********************************************************
*** 原点回零命令
**********************************************************/

// 设置单圈回零的零点位置 - 加载到多电机指令上
// 触发回零 - 加载到多电机指令上
// 强制中断并退出回零 - 加载到多电机指令上
// 修改回零参数 - 加载到多电机指令上

void X_V2_MMCL_Origin_Set_O(u8 addr, bool svF);
void X_V2_MMCL_Origin_Trigger_Return(u8 addr, u8 o_mode, bool snF);
void X_V2_MMCL_Origin_Interrupt(u8 addr);
void X_V2_MMCL_Origin_Modify_Params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);

/**********************************************************
*** 读取系统参数命令
**********************************************************/

// 定时返回信息命令（X42S/Y42） - 加载到多电机指令上
// 读取系统参数 - 加载到多电机指令上

void X_V2_MMCL_Auto_Return_Sys_Params_Timed(u8 addr, zdtSysParams_e s, u16 time_ms);
void X_V2_MMCL_Read_Sys_Params(u8 addr, zdtSysParams_e s);

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

#endif

#endif
