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
 * \brief  USART3 中断处理函数，只缓存包内容
 */
void USART3_IRQHandler(void)
{
    static BYTE packetStarted = 0;

    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {

        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

/**
 * \brief  USART2中断处理函数
 */
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {

        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

/**
 * \brief  USART1中断处理函数
 */
void USART1_IRQHandler(void)
{
    static BYTE packetStarted = 0;

    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {

        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
