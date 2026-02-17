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
        // 串口
        // if (serialTimeFlag)
        //     serialTime++;

        // 按键
        if (keyBox[2])
            keyBox[2]++;
        if (keyBox[2] == 100) { // 1s 后检查
            key_action();
            keyBox[2] = 0; // 关闭自增
        }

        // 时间刷新
        sysTime++;
        // whlTime++;

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

// /******************************************************************
//  * @fn     USART1_IRQHandler
//  * @brief  该函数处理 USART1 中断请求
//  */
// void USART1_IRQHandler(void)
// {
//     if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
//         u8 rxData = USART_ReceiveData(USART1);
//         serial_printf(USART1, "%c", rxData);
//         USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//     }
// }

/******************************************************************
 * \brief  USART1 中断请求处理函数
 */
void USART1_IRQHandler(void)
{
    volatile static u8 rxIdx = 0;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART1);
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
            } else if (srlCmdBuf[0] < SRL_CMDBUF_LEN - 1) {
                srlCmdBuf[++srlCmdBuf[0]] = rxData;
            }
        } else if (rxIdx == 3) {
            if (rxData == '>') {
                srlPidBuf[srlPidBuf[0] + 1] = '\0'; // 字符串结束符
                srlPidFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPidBuf[0] < SRL_PIDBUF_LEN - 1) {
                srlPidBuf[++srlPidBuf[0]] = rxData;
            }
        } else
            serial_printf(4, "%c", rxData); // 转发命令

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * \brief  USART3 中断请求处理函数
 */
void USART3_IRQHandler(void)
{
    volatile static u8 rxIdx = 0;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART3);
        if (rxData == '{') {
            rxIdx        = 1;
            srlSigBuf[0] = 0; // 长度位清零
        } else if (rxData == '[') {
            rxIdx        = 4;
            srlPkgBuf[0] = 0; // 长度位清零
        }

        else if (rxIdx == 1) {
            if (rxData == '}') {
                srlSigBuf[srlSigBuf[0] + 1] = '\0'; // 字符串结束符
                srlSigFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlSigBuf[0] < SRL_SIGBUF_LEN - 1) {
                srlSigBuf[++srlSigBuf[0]] = rxData;
            }
        } else if (rxIdx == 4) {
            if (rxData == ']') {
                srlPkgBuf[srlPkgBuf[0] + 1] = '\0'; // 字符串结束符
                srlPkgFlg                   = 1;    // 接收完成标志
                rxIdx                       = 0;
            } else if (srlPkgBuf[0] < SRL_PKGBUF_LEN - 1) {
                srlPkgBuf[++srlPkgBuf[0]] = rxData;
            }
        }

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

/******************************************************************
 * \brief  UART4 中断请求处理函数
 */
void UART4_IRQHandler(void)
{
    volatile static u8 rxIdx = 0;
    if (USART_GetITStatus(UART4, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(UART4);
        serial_printf(1, "%c", rxData); // 调试输出
        USART_ClearITPendingBit(UART4, USART_IT_RXNE);
    }
}
/* ******************** USART 中断请求 */
