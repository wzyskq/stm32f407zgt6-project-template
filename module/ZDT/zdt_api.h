/******************************************************************
 ** 文件说明：
 ** \brief  本文件（zdt_api.c/.h）主要功能为处理电机返回数据包的接收、解析
 **
 ** \pre    需配合 Emm_V5.c/.h 使用
 **
 ** \note   - 张大头闭环步进电机本身就是集成好的闭环系统，一般情况下直接用 Emm_V5.c/.h 提供的命令控制电机即可.
 **           但如果需要对电机返回的数据包进行分析处理就比较麻烦，所以笔者就封装了这一套 API 来处理电机返回数据包的接收和解析.
 **           使用时只需在相应的 USARTx 和 TIMx 中断服务函数中调用 zdt_irqHandler 和 zdt_irqEndHandler 函数.
 **           在需要的循环中调用 Emm_V5_Get_Sys_Params 函数即可获取解析后的数据包内容.
 **
 **         - zdt_irqEndHandler() 中的最大超时时长由定时中断时基和最大超时周期两者乘积决定.
 **
 **         - Emm_V5_Get_Sys_Params() 返回值为一个伪空安全类型 _s32.
 **           即当返回值的 isNull 为 true 时，表示数据包无效或解析失败；反之时，v 成员即为解析后的数值.
 **
 **         - 强烈建议在每次调用 Emm_V5.c/.h 命令前先判断 zdtTxFlg 是否为 true 来避免数据冲突.
 **           即使不用 Emm_V5_Get_Sys_Params() 读取数据，也建议在定时中断中调用以消耗 zdtFlg 来保持命令流畅调用.
 **  
 ** \example  // 放入中断后可在逻辑中直接连续设定不同的命令而不产生报错
 **           while (zdtTvFlg); // 等待清零
 **           Emm_V5_Vel_Control(4, 1, 0, 60, 0, false);
 **           while (zdtTvFlg); // 等待清零
 **           Emm_V5_Vel_Control(4, 1, 0, 30, 0, false); 
 */

#ifndef __ZDT_API_H
#define __ZDT_API_H

#include "Emm_V5.h"
#include "main.h"

/* Global Macros ----------------------------------------------------------- */

#define ZDT_NUM 2        // 电机数量
#define ZDT_TIMEOUT 1    // 定时器超时周期
#define ZDT_RVBUF_LEN 64 // 缓存数据包长度

/* Global Types ------------------------------------------------------------ */

typedef struct
{
    bool isNull;
    s32 v;
} _s32; // 伪空安全 s32

/* Global Variables -------------------------------------------------------- */

extern __IO u8 zdtRxIdx;
extern __IO u8 zdtTimCnt;
extern __IO bool zdtTimFlg;
extern __IO bool zdtRvFlg;

extern __IO bool zdtTvFlg;
extern __IO u8 zdtRvBuf[];
extern u8 zdtTvTag[];

/* Global Functions -------------------------------------------------------- */

extern void zdt_irqHandler(USART_TypeDef *usart);
extern void zdt_irqEndHandler(void);
extern _s32 Emm_V5_Get_Sys_Params(void);

#endif
