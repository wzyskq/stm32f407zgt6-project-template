#include "stm32f4xx_it.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern uint16_t Serial_RxData;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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


/**
 * @brief  USART3 中断处理函数，只缓存包内容
 */
void USART3_IRQHandler(void)
{
    static BYTE packetStarted = 0;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        BYTE RxData = USART_ReceiveData(USART3);
        // serial_send_byte(USART2, RxData); // 回显到调试串口

        if (RxData == '{')
        {
            packetStarted = 1;
            serialSignBuf[0] = 0; // 内容长度清零（固定为 1）
        }
        else if (RxData == '<'){
            packetStarted = 2;
            serialPacketBuf[0] = 0; // 内容长度清零（固定为 2）
        }

        // 处理不同状态下的数据接收
        else if (packetStarted == 1)
        {
            if (RxData == '}')
            {
                serialSignOk = 1; // 数据包接收完成
                packetStarted = 0;
            }
            else if (serialSignBuf[0] < SERIAL_BUF_SIZE - 1)
            {
                serialSignBuf[++serialSignBuf[0]] = RxData; // 只存内容
            }
        }
        else if (packetStarted == 2)
        {
            if (RxData == '>')
            {
                serialPacketOk = 1; // 数据包接收完成
                packetStarted = 0;
            }
            else if (serialPacketBuf[0] < SERIAL_BUF_SIZE - 1)
            {
                serialPacketBuf[++serialPacketBuf[0]] = RxData; // 只存内容
            }
        }
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// /**
//  * @brief  USART2中断处理函数
//  * @note   处理USART2接收数据并回显到调试串口
//  */
// void USART2_IRQHandler(void)
// {
//     if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
//     {
//         BYTE RxData = USART_ReceiveData(USART2);
//         serial_send_byte(USART3, RxData); // 回显到调试串口
//         // serial_send_byte(USART1, RxData); // 回显到调试串口
//         USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//     }
// }

/**
 * @brief  USART1中断处理函数
 * @note   处理USART1接收数据并回显到调试串口
 */
void USART1_IRQHandler(void)
{
    static BYTE packetStarted = 0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        BYTE RxData = USART_ReceiveData(USART1);
        // serial_send_byte(USART2, RxData); // 回显到调试串口

        if (RxData == '<')
        {
            packetStarted = 1;                           // 开始接收PID数据包
            memset(serialPIDBuf, 0, SERIAL_BUFFER_SIZE); // 清除内容
        }
        else if (RxData == '{')
        {
            packetStarted = 2; // 开始接收转发数据包
        }
        else if (RxData == '(')
        {
            packetStarted = 3;
            memset(serialCMDBuf, 0, SERIAL_BUFFER_SIZE);
        }

        // 处理不同状态下的数据接收
        else if (packetStarted == 1)
        {
            if (RxData == '>')
            {
                serialPIDOk = 1; // 数据包接收完成
                packetStarted = 0;
            }
            else if (serialPIDBuf[0] < SERIAL_BUFFER_SIZE - 1)
            {
                serialPIDBuf[++serialPIDBuf[0]] = RxData;
            }
        }
        else if (packetStarted == 2)
        {
            if (RxData == '}')
            {
                packetStarted = 0; // 结束转发视觉
            }
            else
            {
                serial_printf(USART3, "{%c}", RxData);
            }
        }
        else if (packetStarted == 3)
        {
            if (RxData == ')')
            {
                serialCMDOk = 1; // 数据包接收完成
                packetStarted = 0;
            }
            else if (serialCMDBuf[0] < SERIAL_BUFFER_SIZE - 1)
            {
                serialCMDBuf[++serialCMDBuf[0]] = RxData;
            }
        }

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
