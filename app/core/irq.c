#include "irq.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* TIM 中断请求 ******************** */

/******************************************************************
 * \brief  TIM7 中断请求处理函数
 * \note   系统任务调度时基（中断周期 10ms）
 */
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {
        // 时间刷新
        ++sysTime;

        // 步进电机
        zdt_irqEndHandler();                // 电机中断请求处理
        Emm_V5_Get_Sys_Params(&zdtSysData); // 消费标志位 zdtTvFlg

//         u32 t = sysTime / 100;
// 
//         if (t % 2 == 0) {
//             led_on(led0);
//         } else {
//             led_off(led0);
//         }

        // 按键
        if (keySts.tim)
            keySts.tim++;
        if (keySts.tim > keySts.overtime) { // 超时后检查
            key_action();
            keySts.tim = 0; // 关闭自增
        }

        // 方向环
        // direction_loop();

        // 位置环
        // position_loop();

        // 速度环
        // speed_loop();

        // if (spdLogFlag)
        // serial_printf(1, "spd:%d, %d, %d\n", whlSpd[0], whlCnt[0], whlCnt[1]);
        // serial_printf(1, "spd:%d, %d, %d, %d\n", tWhlVn, whlCnt[0], whlCnt[1], whlSpd[0]); // 速度调试
        // serial_printf(1, "dir:%d, %d\n", carDeg, mpuYaw.yaw_z); // 方向调试

        // pass

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}

/* ******************** TIM 中断请求 */

/*





*/

/* USART 中断请求 ******************** */

/******************************************************************
 * @fn     USART1_IRQHandler
 * @brief  该函数处理 USART1 中断请求
 */
void USART1_IRQHandler(void)
{
    // if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
    //     u8 rxData = USART_ReceiveData(USART1);
    //     serial_printf(usart1, "%c", rxData);
    //     USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    // }

    __IO static u8 rxIdx = 0;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART1);
        if (rxData == '(') {
            rxIdx        = 2;
            srlCmdBuf[0] = 0; // 长度位清零
        } else if (rxData == '<') {
            rxIdx        = 3;
            srlPidBuf[0] = 0; // 长度位清零
        } else if (rxData == 0xEE) {
            rxIdx = 5;
        }

        else if (rxIdx == 2) {
            if (rxData == ')') {
                srlCmdBuf[srlCmdBuf[0] + 1] = '\0'; // 字符串结束符
                srlCmdFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlCmdBuf[0] < SRL_BUF_LLEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxIdx == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        } else if (rxIdx == 5) {
            serial_send_byte(ZDT_SRL, rxData); // 转发命令
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART2_IRQHandler
 * @brief  该函数处理 USART2 中断请求
 */
void USART2_IRQHandler(void)
{
    __IO static u8 rxIdx = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART2);
        if (rxData == '(') {
            rxIdx        = 2;
            srlCmdBuf[0] = 0; // 长度位清零
        } else if (rxData == '<') {
            rxIdx        = 3;
            srlPidBuf[0] = 0; // 长度位清零
        }

        else if (rxIdx == 2) {
            if (rxData == ')') {
                srlCmdBuf[srlCmdBuf[0] + 1] = '\0'; // 字符串结束符
                srlCmdFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlCmdBuf[0] < SRL_BUF_LLEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxIdx == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPidBuf[0] < SRL_BUF_MLEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/******************************************************************
 * \brief  UART4 中断请求处理函数
 */
void UART4_IRQHandler(void)
{
    zdt_irqHandler(UART4);
}

/* ******************** USART 中断请求 */

/*





*/

/* SDIO 中断请求 ******************** */

/******************************************************************
 * \brief  SDIO 中断请求处理函数
 */
void SDIO_IRQHandler(void)
{
    /* Process All SDIO Interrupt Sources */
    SD_ProcessIRQSrc();
}

/******************************************************************
 * \brief  SDIO DMA 中断请求处理函数
 */
void SD_SDIO_DMA_IRQHANDLER(void)
{
    /* Process DMA2 Stream3 or DMA2 Stream6 Interrupt Sources */
    SD_ProcessDMAIRQ();
}

/* ******************** SDIO 中断请求 */
