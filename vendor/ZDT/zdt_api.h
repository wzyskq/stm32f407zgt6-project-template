/******************************************************************
 ** \file    zdt_api.h
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

#ifndef __ZDT_API_H
#define __ZDT_API_H

#include "heads.h"
#include "zdt_com.h"
#include "zdt_pro.h"

/* 固件选择 */

#if !defined(ZDT_EMM_V5) && !defined(ZDT_X_V2)

#define ZDT_EMM_V5 // Emm 固件
// #define ZDT_X_V2   // X 固件

#endif

/* 重复定义检查 */

#if defined(ZDT_EMM_V5) && defined(ZDT_X_V2)
#error "请仅定义 ZDT_EMM_V5 或 ZDT_X_V2 中的一个，当前定义了两个，请检查 zdt_api.h 文件顶部的宏定义部分！"
#endif

/* 文件引用 */

#ifdef ZDT_EMM_V5
#include "Emm_V5.h"
#elif defined(ZDT_X_V2)
#include "X_V2.h"
#endif


/* Global Macros ----------------------------------------------------------- */

#define ZDT_NUM           2      // 电机数量
#define ZDT_SRL           uart4  // 电机串口号
#define ZDT_RESRL         usart1 // 调试返回串口号
#define ZDT_TIMEOUT       8      // 接收超时时长（ms）
#define ZDT_RVBUF_LEN     64     // 缓存数据包长度
#define ZDT_IRQEND_PERIOD 1      // 结束接收超时判断周期

/* Private Types ----------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

void zdt_trig_encoder_cal(srl_e idx, u8 addr);
void zdt_reset_motor(srl_e idx, u8 addr);
void zdt_reset_curpos_to_zero(srl_e idx, u8 addr);
void zdt_reset_clog_pro(srl_e idx, u8 addr);
void zdt_restore_motor(srl_e idx, u8 addr);
void zdt_multi_motor_cmd(srl_e idx, u8 addr);
void zdt_en_control(srl_e idx, u8 addr, bool state, bool snF);
void zdt_vel_control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, bool snF);
void zdt_pos_control(srl_e idx, u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF);
void zdt_stop_now(srl_e idx, u8 addr, bool snF);
void zdt_synchronous_motion(srl_e idx, u8 addr);
void zdt_origin_set_o(srl_e idx, u8 addr, bool svF);
void zdt_origin_trigger_return(srl_e idx, u8 addr, u8 o_mode, bool snF);
void zdt_origin_interrupt(srl_e idx, u8 addr);
void zdt_origin_read_params(srl_e idx, u8 addr);
void zdt_origin_modify_params(srl_e idx, u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);
void zdt_auto_return_sys_params_timed(srl_e idx, u8 addr, zdtSysParams_e s, u16 time_ms);
void zdt_read_sys_params(srl_e idx, u8 addr, zdtSysParams_e s);
void zdt_modify_motor_id(srl_e idx, u8 addr, bool svF, u8 id);
void zdt_modify_micro_step(srl_e idx, u8 addr, bool svF, u8 mstep);
void zdt_modify_pdflag(srl_e idx, u8 addr, bool pdf);
void zdt_read_opt_param_sta(srl_e idx, u8 addr);
void zdt_modify_motor_type(srl_e idx, u8 addr, bool svF, bool mottype);
void zdt_modify_firmware_type(srl_e idx, u8 addr, bool svF, bool fwtype);
void zdt_modify_ctrl_mode(srl_e idx, u8 addr, bool svF, bool ctrl_mode);
void zdt_modify_motor_dir(srl_e idx, u8 addr, bool svF, bool dir);
void zdt_modify_lock_btn(srl_e idx, u8 addr, bool svF, bool lockbtn);
void zdt_modify_s_vel(srl_e idx, u8 addr, bool svF, bool s_vel);
void zdt_modify_om_ma(srl_e idx, u8 addr, bool svF, u16 om_ma);
void zdt_modify_foc_ma(srl_e idx, u8 addr, bool svF, u16 foc_mA);
void zdt_read_pid_params(srl_e idx, u8 addr);
void zdt_modify_pid_params(srl_e idx, u8 addr, bool svF, u32 kp, u32 ki, u32 kd);
void zdt_read_dmx512_params(srl_e idx, u8 addr);
void zdt_modify_dmx512_params(srl_e idx, u8 addr, bool svF, u16 tch, u8 nch, u8 mode, u16 vel, u16 acc, u16 vel_step, u32 pos_step);
void zdt_read_pos_window(srl_e idx, u8 addr);
void zdt_modify_pos_window(srl_e idx, u8 addr, bool svF, u16 prw);
void zdt_read_otocp(srl_e idx, u8 addr);
void zdt_modify_otocp(srl_e idx, u8 addr, bool svF, u16 otp, u16 ocp, u16 time_ms);
void zdt_read_heart_protect(srl_e idx, u8 addr);
void zdt_modify_heart_protect(srl_e idx, u8 addr, bool svF, u32 hp);
void zdt_read_integral_limit(srl_e idx, u8 addr);
void zdt_modify_integral_limit(srl_e idx, u8 addr, bool svF, u32 il);
void zdt_read_system_state_params(srl_e idx, u8 addr);
void zdt_read_motor_conf_params(srl_e idx, u8 addr);
void zdt_mmcl_trig_encoder_cal(u8 addr);
void zdt_mmcl_reset_motor(u8 addr);
void zdt_mmcl_reset_curpos_to_zero(u8 addr);
void zdt_mmcl_reset_clog_pro(u8 addr);
void zdt_mmcl_restore_motor(u8 addr);
void zdt_mmcl_en_control(u8 addr, bool state, bool snF);
void zdt_mmcl_vel_control(u8 addr, u8 dir, u16 vel, u8 acc, bool snF);
void zdt_mmcl_pos_control(u8 addr, u8 dir, u16 vel, u8 acc, u32 clk, bool raF, bool snF);
void zdt_mmcl_stop_now(u8 addr, bool snF);
void zdt_mmcl_synchronous_motion(u8 addr);
void zdt_mmcl_origin_set_o(u8 addr, bool svF);
void zdt_mmcl_origin_trigger_return(u8 addr, u8 o_mode, bool snF);
void zdt_mmcl_origin_interrupt(u8 addr);
void zdt_mmcl_origin_modify_params(u8 addr, bool svF, u8 o_mode, u8 o_dir, u16 o_vel, u32 o_tm, u16 sl_vel, u16 sl_ma, u16 sl_ms, bool potF);
void zdt_mmcl_auto_return_sys_params_timed(u8 addr, zdtSysParams_e s, u16 time_ms);
void zdt_mmcl_read_sys_params(u8 addr, zdtSysParams_e s);

#endif
