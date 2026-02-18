#ifndef __EMM_V5_H
#define __EMM_V5_H

#include "main.h"

/**********************************************************
***	Emm_V5.0步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

/* Global Macros ----------------------------------------------------------- */

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MMCL_LEN 512

/* Global Types ------------------------------------------------------------ */

typedef enum
{
    S_VBUS = 5,   // 读取总线电压
    S_CBUS = 6,   // 读取总线电流
    S_CPHA = 7,   // 读取相电流
    S_ENCO = 8,   // 读取编码器原始值
    S_CLKC = 9,   // 读取实时脉冲数
    S_ENCL = 10,  // 读取经过线性化校准后的编码器值
    S_CLKI = 11,  // 读取输入脉冲数
    S_TPOS = 12,  // 读取电机目标位置
    S_SPOS = 13,  // 读取电机实时设定的目标位置
    S_VEL = 14,   // 读取电机实时转速
    S_CPOS = 15,  // 读取电机实时位置
    S_PERR = 16,  // 读取电机位置误差
    S_VBAT = 17,  // 读取多圈编码器电池电压（Y42）
    S_TEMP = 18,  // 读取电机实时温度（Y42）
    S_FLAG = 19,  // 读取电机状态标志位
    S_OFLAG = 20, // 读取回零状态标志位
    S_OAF = 21,   // 读取电机状态标志位 + 回零状态标志位（Y42）
    S_PIN = 22,   // 读取引脚状态（Y42）
} SysParams_t;

/* Global Variables -------------------------------------------------------- */

extern __IO u16 MMCL_count, MMCL_cmd[MMCL_LEN];

/* Global Functions -------------------------------------------------------- */

/**
***********************************************************
***********************************************************
***
***
*** @brief	后缀带有（Y42）为Y42新增命令，X42不能用，其他通用
***
***
***********************************************************
***********************************************************
***/

/**********************************************************
*** 触发动作命令
**********************************************************/

// 触发编码器校准
// 重启电机（Y42）
// 将当前位置清零
// 解除堵转保护
// 恢复出厂设置

void Emm_V5_Trig_Encoder_Cal(u8 srlNum, u8 addr);
void Emm_V5_Reset_Motor(u8 srlNum, u8 addr);
void Emm_V5_Reset_CurPos_To_Zero(u8 srlNum, u8 addr);
void Emm_V5_Reset_Clog_Pro(u8 srlNum, u8 addr);
void Emm_V5_Restore_Motor(u8 srlNum, u8 addr);

/**********************************************************
*** 运动控制命令
**********************************************************/

// 多电机命令（Y42）
// 电机使能控制
// 速度模式控制
// 位置模式控制
// 让电机立即停止运动
// 触发多机同步开始运动

void Emm_V5_Multi_Motor_Cmd(u8 srlNum, u8 addr);
void Emm_V5_En_Control(u8 srlNum, u8 addr, bool state, bool snF);
void Emm_V5_Vel_Control(u8 srlNum, u8 addr, u8 dir, u16 vel, u8 acc, bool snF); 
void Emm_V5_Pos_Control(u8 srlNum, u8 addr, u8 dir, u16 vel, u8 acc, uint32_t clk, bool raF, bool snF); 
void Emm_V5_Stop_Now(u8 srlNum, u8 addr, bool snF);
void Emm_V5_Synchronous_motion(u8 srlNum, u8 addr);

/**********************************************************
*** 原点回零命令
**********************************************************/

// 设置单圈回零的零点位置
// 触发回零
// 强制中断并退出回零
// 读取回零参数
// 修改回零参数

void Emm_V5_Origin_Set_O(u8 srlNum, u8 addr, bool svF);
void Emm_V5_Origin_Trigger_Return(u8 srlNum, u8 addr, u8 o_mode, bool snF);
void Emm_V5_Origin_Interrupt(u8 srlNum, u8 addr);
void Emm_V5_Origin_Read_Params(u8 srlNum, u8 addr);
void Emm_V5_Origin_Modify_Params(u8 srlNum, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, uint32_t o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);

/**********************************************************
*** 读取系统参数命令
**********************************************************/

// 定时返回信息命令（Y42） 
// 读取系统参数

void Emm_V5_Auto_Return_Sys_Params_Timed(u8 srlNum, u8 addr, SysParams_t s, u16 time_ms);
void Emm_V5_Read_Sys_Params(u8 srlNum, u8 addr, SysParams_t s);

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

// 修改电机ID地址
// 修改细分值
// 修改掉电标志
// 读取选项参数状态（Y42）
// 修改电机类型（Y42）
// 修改固件类型（Y42）

void Emm_V5_Modify_Motor_ID(u8 srlNum, u8 addr, bool svF, u8 id);
void Emm_V5_Modify_MicroStep(u8 srlNum, u8 addr, bool svF, u8 mstep);
void Emm_V5_Modify_PDFlag(u8 srlNum, u8 addr, bool pdf);
void Emm_V5_Read_Opt_Param_Sta(u8 srlNum, u8 addr);
void Emm_V5_Modify_Motor_Type(u8 srlNum, u8 addr, bool svF, bool mottype);
void Emm_V5_Modify_Firmware_Type(u8 srlNum, u8 addr, bool svF, bool fwtype);

// 修改开环/闭环控制模式（Y42）
// 修改电机运动正方向（Y42）
// 修改锁定按键功能（Y42）
// 修改命令速度值是否缩小10倍输入（Y42）
// 修改开环模式工作电流
// 修改闭环模式最大电流

void Emm_V5_Modify_Ctrl_Mode(u8 srlNum, u8 addr, bool svF, bool ctrl_mode);
void Emm_V5_Modify_Motor_Dir(u8 srlNum, u8 addr, bool svF, bool dir);
void Emm_V5_Modify_Lock_Btn(u8 srlNum, u8 addr, bool svF, bool lockbtn);
void Emm_V5_Modify_S_Vel(u8 srlNum, u8 addr, bool svF, bool s_vel);
void Emm_V5_Modify_OM_mA(u8 srlNum, u8 addr, bool svF, u16 om_ma);
void Emm_V5_Modify_FOC_mA(u8 srlNum, u8 addr, bool svF, u16 foc_mA);

// 读取PID参数
// 修改PID参数
// 读取DMX512协议参数（Y42）
// 修改DMX512协议参数（Y42）
// 读取位置到达窗口（Y42）
// 修改位置到达窗口（Y42）

void Emm_V5_Read_PID_Params(u8 srlNum, u8 addr);
void Emm_V5_Modify_PID_Params(u8 srlNum, u8 addr, bool svF, uint32_t kp, uint32_t ki, uint32_t kd);
void Emm_V5_Read_DMX512_Params(u8 srlNum, u8 addr);
void Emm_V5_Modify_DMX512_Params(u8 srlNum, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, uint32_t pos_step);
void Emm_V5_Read_Pos_Window(u8 srlNum, u8 addr);
void Emm_V5_Modify_Pos_Window(u8 srlNum, u8 addr, bool svF, u16 prw);

// 读取过热过流保护检测阈值（Y42）
// 修改过热过流保护检测阈值（Y42）
// 读取心跳保护功能时间（Y42）
// 修改心跳保护功能时间（Y42）
// 读取积分限幅/刚性系数（Y42）
// 修改积分限幅/刚性系数（Y42）

void Emm_V5_Read_Otocp(u8 srlNum, u8 addr);
void Emm_V5_Modify_Otocp(u8 srlNum, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms);
void Emm_V5_Read_Heart_Protect(u8 srlNum, u8 addr);
void Emm_V5_Modify_Heart_Protect(u8 srlNum, u8 addr, bool svF, uint32_t hp);
void Emm_V5_Read_Integral_Limit(u8 srlNum, u8 addr);
void Emm_V5_Modify_Integral_Limit(u8 srlNum, u8 addr, bool svF, uint32_t il);

/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/

// 读取系统状态参数
// 读取驱动配置参数
void Emm_V5_Read_System_State_Params(u8 srlNum, u8 addr);
void Emm_V5_Read_Motor_Conf_Params(u8 srlNum, u8 addr);

/**
***********************************************************
***********************************************************
***
***
*** @brief	以下是把相应命令加载到Y42多电机命令上的函数（Y42）
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

void Emm_V5_MMCL_Trig_Encoder_Cal(u8 addr);
void Emm_V5_MMCL_Reset_Motor(u8 addr);
void Emm_V5_MMCL_Reset_CurPos_To_Zero(u8 addr);
void Emm_V5_MMCL_Reset_Clog_Pro(u8 addr);
void Emm_V5_MMCL_Restore_Motor(u8 addr);

/**********************************************************
*** 运动控制命令
**********************************************************/

// 电机使能控制 - 加载到多电机指令上
// 速度模式控制 - 加载到多电机指令上
// 位置模式控制 - 加载到多电机指令上
// 让电机立即停止运动 - 加载到多电机指令上
// 触发多机同步开始运动 - 加载到多电机指令上

void Emm_V5_MMCL_En_Control(u8 addr, bool state, bool snF);
void Emm_V5_MMCL_Vel_Control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF);
void Emm_V5_MMCL_Pos_Control(u8 addr, u8 dir, u16 vel, u8 acc, uint32_t clk, bool raF, bool snF);
void Emm_V5_MMCL_Stop_Now(u8 addr, bool snF);
void Emm_V5_MMCL_Synchronous_motion(u8 addr);

/**********************************************************
*** 原点回零命令
**********************************************************/

// 设置单圈回零的零点位置 - 加载到多电机指令上
// 触发回零 - 加载到多电机指令上
// 强制中断并退出回零 - 加载到多电机指令上
// 修改回零参数 - 加载到多电机指令上

void Emm_V5_MMCL_Origin_Set_O(u8 addr, bool svF);
void Emm_V5_MMCL_Origin_Trigger_Return(u8 addr, u8 o_mode, bool snF);
void Emm_V5_MMCL_Origin_Interrupt(u8 addr);
void Emm_V5_MMCL_Origin_Modify_Params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, uint32_t o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);

/**********************************************************
*** 读取系统参数命令
**********************************************************/

// 定时返回信息命令（Y42） - 加载到多电机指令上
// 读取系统参数 - 加载到多电机指令上

void Emm_V5_MMCL_Auto_Return_Sys_Params_Timed(u8 addr, SysParams_t s, u16 time_ms);
void Emm_V5_MMCL_Read_Sys_Params(u8 addr, SysParams_t s);

/**********************************************************
*** 读写驱动参数命令
**********************************************************/

#endif
