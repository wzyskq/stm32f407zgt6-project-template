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
 **         - Emm_V5_Get_Sys_Params() 将解析结果写入传入的 zdtSysData_t 指针，调用前应保证指针有效.
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
    float vbus;  // 读取总线电压
    float cbus;  // 读取总线电流
    float cpha;  // 读取相电流
    float enco;  // 读取编码器原始值
    float clkc;  // 读取实时脉冲数
    float encl;  // 读取经过线性化校准后的编码器值
    float clki;  // 读取输入脉冲数
    float tpos;  // 读取电机目标位置
    float spos;  // 读取电机实时设定的目标位置
    float vel;   // 读取电机实时转速
    float cpos;  // 读取电机实时位置
    float perr;  // 读取电机位置误差
    float vbat;  // 读取多圈编码器电池电压（Y42）
    float temp;  // 读取电机实时温度（Y42）
    float flag;  // 读取电机状态标志位
    float oflag; // 读取回零状态标志位
    float oaf;   // 读取电机状态标志位 + 回零状态标志位（Y42）
    float pin;   // 读取引脚状态（Y42）
} zdtSysData_t;

/* Global Variables -------------------------------------------------------- */

extern zdtSysData_t zdtSysData;

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
extern void Emm_V5_Get_Sys_Params(zdtSysData_t *data);

#endif
