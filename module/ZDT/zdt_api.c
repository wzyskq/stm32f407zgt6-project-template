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

#include "zdt_api.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

__IO u8 zdtRxIdx    = 0; // 串口接收数据状态机
__IO u8 zdtTimCnt   = 0; // 定时器周期计数变量
__IO bool zdtTimFlg = 0; // 定时器周期标志位
__IO bool zdtRvFlg  = 0; // 串口返回数据标志位

__IO bool zdtTvFlg              = 0;   // 串口发送数据标志位
__IO u8 zdtRvBuf[ZDT_RVBUF_LEN] = {0}; // 串口返回数据包（有效值从 1 开始）
u8 zdtTvTag[ZDT_NUM]            = {0}; // 串口返回数据信息 [地址, 功能码]

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief      电机中断请求处理
 * \param[in]  usart USARTx
 * \note       在 USARTx 中断服务函数中调用
 * \example    // 假设用于 USART1：
 *              void USART1_IRQHandler(void)
 *              {
 *                  zdt_irqHandler(USART1);
 *              }
 * \warning    严禁在此函数内调用任何阻塞式函数，否则会导致丢包等一系列问题！！！
 */
void zdt_irqHandler(USART_TypeDef *usart)
{
    if (USART_GetITStatus(usart, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(usart);
        if (!zdtRxIdx) {
            zdtRxIdx    = 1;
            zdtRvBuf[0] = 0; // 长度位清零
        }

        if (zdtRxIdx == 1) {
            if (zdtRvBuf[0] < ZDT_RVBUF_LEN - 1) { // 确保不会越界
                zdtRvBuf[++zdtRvBuf[0]] = rxData;

                if (rxData == 0x6B) { // 0x6B 触发结束计时
                    zdtTimCnt = 0;
                    zdtTimFlg = true; // 开始计时
                } else {
                    zdtTimFlg = false; // 取消计时
                }
            } else {
                zdtRxIdx = 0; // 丢弃过长数据包
            }
        }

        USART_ClearITPendingBit(usart, USART_IT_RXNE);
    }
}

/******************************************************************
 * \brief      电机终止中断请求处理
 * \param[in]  srlIdx 串口索引，范围 [0, ZDT_SRL_NUM)
 * \note       在 TIMx 中断服务函数中调用
 * \example    // 假设用于 TIM7：
 *              void TIM7_IRQHandler(void)
 *              {
 *                  if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {
 *                      // ... 其他代码 ...
 *                      zdt_irqEndHandler();
 *                      // ... 其他代码 ...
 *                      TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
 *                  }
 *              }
 */
void zdt_irqEndHandler(void)
{
    if (zdtTimFlg) {
        zdtTimCnt++;
        if (zdtTimCnt >= ZDT_TIMEOUT) { // 最大超时时长 10ms
            zdtTimFlg               = false;
            zdtRvFlg                = true; // 设置数据标志位
            zdtRvBuf[++zdtRvBuf[0]] = '\0'; // 字符串结束符
            zdtRxIdx                = 0;    // 重置接收状态
        }
    }
}

/******************************************************************
 * \brief  解析电机返回数据包内容
 * \note   在主循环中调用，当 zdtRvFlg 为 1 时
 */
_s32 Emm_V5_Get_Sys_Params(void)
{
    if (!zdtRvFlg)
        return (_s32){.isNull = true};

    // serial_printf(1, "ZDT: Rv: %02X %02X %02X %02X\n", zdtRvBuf[1], zdtRvBuf[2], zdtRvBuf[3], zdtRvBuf[4]);

    if (zdtRvBuf[1] != zdtTvTag[0])
        return (_s32){.isNull = true};

    u32 v       = 0;
    _s32 retval = {.isNull = false, .v = 0};

    switch (zdtRvBuf[2]) {
        case 0x24: // S_VBUS 读取总线电压

            break;
        case 0x26: // S_CBUS 读取总线电流

            break;
        case 0x27: // S_CPHA 读取相电流

            break;
        case 0x29: // S_ENCO 读取编码器原始值

            break;
        case 0x30: // S_CLKC 读取实时脉冲数

            break;
        case 0x31: // S_ENCL 读取线性化后编码器值

            break;
        case 0x32: // S_CLKI 读取输入脉冲数

            break;
        case 0x33: // S_TPOS 读取目标位置
            v = (zdtRvBuf[4] << 24) | (zdtRvBuf[5] << 16) | (zdtRvBuf[6] << 8) | zdtRvBuf[7];
            v = (u32)((float)v * 360.0 / 65536.0); // 转换为度数
            if (v > 0x7FFFFFFF)
                retval.isNull = 2; // 伪空，表示数据异常（超过s32范围）
            else
                retval.v = (zdtRvBuf[3] == 0) ? (s32)v : -(s32)v;
            break;
        case 0x34: // S_SPOS 读取实时设定目标位置

            break;
        case 0x35: // S_VEL 读取实时转速
            v        = (zdtRvBuf[4] << 8) | zdtRvBuf[5];
            retval.v = (zdtRvBuf[3] == 0) ? (s32)v : -(s32)v;
            break;
        case 0x36: // S_CPOS 读取实时位置

            break;
        case 0x37: // S_PERR 读取位置误差

            break;
        case 0x38: // S_VBAT 读取多圈编码器电池电压（Y42）

            break;
        case 0x39: // S_TEMP 读取实时温度（Y42）

            break;
        case 0x3A: // S_FLAG 读取状态标志位

            break;
        case 0x3B: // S_OFLAG 读取回零状态标志位

            break;
        case 0x3C: // S_OAF 读取状态+回零标志位（Y42）

            break;
        case 0x3D: // S_PIN 读取引脚状态（Y42）

            break;
        default:                   // 非读取命令的返回数据包/未知标签
            zdtTvFlg      = false; // 直接消费，避免阻塞读取进程
            retval.isNull = true;
            if (zdtRvBuf[3] != 0x02)
                serial_printf(1, "ZDT: Err: %02X %02X %02X\n", zdtRvBuf[1], zdtRvBuf[2], zdtRvBuf[3]);
            break;
    }

    // zdtTvFlg = 0; // 注意：请在消费后清除标志位
    zdtRvFlg = false;
    return retval;
}
