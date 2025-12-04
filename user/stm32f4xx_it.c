#include "stm32f4xx_it.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef --------------------------------------------------------- */

/* Private define ---------------------------------------------------------- */

/* Private macro ----------------------------------------------------------- */

/* Global variables ------------------------------------------------------- */

extern uint16_t Serial_RxData;

/* Global functions ------------------------------------------------------- */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

// /**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
// void SysTick_Handler(void)
// {
//     TimingDelay_Decrement();
// }

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
void PPP_IRQHandler(void)
{
}

// /**
//  * @brief  Decrement the TimingDelay variable.
//  * @param  None
//  * @retval None
//  */
// static __IO uint32_t TimingDelay;
// 
// void TimingDelay_Decrement(void)
// {
//     if (TimingDelay != 0x00)
//     {
//         TimingDelay--;
//     }
// }

/******************************************************************
 * @fn     USART1_IRQHandler
 * @brief  该函数处理 USART1 中断请求
 */
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        u8 rxData = USART_ReceiveData(USART1);
        serial_printf(USART1, "%c", rxData);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART2_IRQHandler
 * @brief  该函数处理 USART2 中断请求
 */
void USART2_IRQHandler(void)
{
    volatile static u8 rxIdx = 0;
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
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/******************************************************************
 * @fn     USART3_IRQHandler
 * @brief  该函数处理 USART3 中断请求
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
