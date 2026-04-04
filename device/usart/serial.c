#include "serial.h"

/* Global Macros ----------------------------------------------------------- */

// 详见 serial.h

/* Private Variables ------------------------------------------------------- */

static const srl_s srlList[] = {
    [usart1] = {
        .rccGpio = RCC_AHB1Periph_GPIOA,
        .gpio    = GPIOA,
        .TxRx    = {GPIO_Pin_9, GPIO_Pin_10},
        .srcTxRx = {GPIO_PinSource9, GPIO_PinSource10},
        .af      = GPIO_AF_USART1,
        .rccUart = RCC_APB2Periph_USART1,
        .uart    = USART1,
        .irqn    = USART1_IRQn,
    },
    [usart2] = {
        .rccGpio = RCC_AHB1Periph_GPIOD,
        .gpio    = GPIOD,
        .TxRx    = {GPIO_Pin_5, GPIO_Pin_6},
        .srcTxRx = {GPIO_PinSource5, GPIO_PinSource6},
        .af      = GPIO_AF_USART2,
        .rccUart = RCC_APB1Periph_USART2,
        .uart    = USART2,
        .irqn    = USART2_IRQn,
    },
    [usart3] = {
        .rccGpio = RCC_AHB1Periph_GPIOD,
        .gpio    = GPIOD,
        .TxRx    = {GPIO_Pin_8, GPIO_Pin_9},
        .srcTxRx = {GPIO_PinSource8, GPIO_PinSource9},
        .af      = GPIO_AF_USART3,
        .rccUart = RCC_APB1Periph_USART3,
        .uart    = USART3,
        .irqn    = USART3_IRQn,
    },
    [uart4] = {
        .rccGpio = RCC_AHB1Periph_GPIOA,
        .gpio    = GPIOA,
        .TxRx    = {GPIO_Pin_0, GPIO_Pin_1},
        .srcTxRx = {GPIO_PinSource0, GPIO_PinSource1},
        .af      = GPIO_AF_UART4,
        .rccUart = RCC_APB1Periph_UART4,
        .uart    = UART4,
        .irqn    = UART4_IRQn,
    },
};

/* Global Variables -------------------------------------------------------- */

u8 srlReFlag = 0; // 串口调试返回标志位

// 串口接收缓存

__IO u8 srlSigBuf[SRL_BUF_SLEN] = {0}; // 标志位缓存
__IO u8 srlSigFlg               = 0;
__IO u8 srlPidBuf[SRL_BUF_MLEN] = {0}; // PID 缓存
__IO u8 srlPidFlg               = 0;
__IO u8 srlCmdBuf[SRL_BUF_LLEN] = {0}; // 命令缓存
__IO u8 srlCmdFlg               = 0;
__IO u8 srlPkgBuf[SRL_BUF_LLEN] = {0}; // 数据包缓存
__IO u8 srlPkgFlg               = 0;

/* Global Functions -------------------------------------------------------- */

/* 发送函数 ******************** */

/******************************************************************
 * \brief      串口初始化
 * \param[in]  idx 串口索引
 * \param[in]  baudRate 波特率
 * \param[in]  priority 优先级，格式：抢占优先级+响应优先级. 例：01 表示抢占优先级0，响应优先级1
 * \note       请确保私有量 srlList 已正确配置
 */
void serial_init(srl_e idx, u32 baudRate, u8 priority)
{
    /* 参数检查 */
    if (idx > usart6) return;

    /* 开启时钟 */
    if (srlList[idx].rccUart == RCC_APB2Periph_USART1 || srlList[idx].rccUart == RCC_APB2Periph_USART6)
        RCC_APB2PeriphClockCmd(srlList[idx].rccUart, ENABLE); // USART1/6 在 APB2
    else
        RCC_APB1PeriphClockCmd(srlList[idx].rccUart, ENABLE); // USART2/3、UART4/5 在 APB1
    RCC_AHB1PeriphClockCmd(srlList[idx].rccGpio, ENABLE);     // GPIOx 在 AHB1

    /* GPIO 初始化 */
    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Pin         = srlList[idx].TxRx[0] | srlList[idx].TxRx[1];
    gpio.GPIO_Mode        = GPIO_Mode_AF;
    gpio.GPIO_Speed       = GPIO_Speed_50MHz;
    gpio.GPIO_OType       = GPIO_OType_PP;
    gpio.GPIO_PuPd        = GPIO_PuPd_UP;
    GPIO_Init((GPIO_TypeDef *)srlList[idx].gpio, &gpio);

    /* 连接引脚USARTx */
    GPIO_PinAFConfig((GPIO_TypeDef *)srlList[idx].gpio, srlList[idx].srcTxRx[0], srlList[idx].af);
    GPIO_PinAFConfig((GPIO_TypeDef *)srlList[idx].gpio, srlList[idx].srcTxRx[1], srlList[idx].af);

    /* USART初始化 */
    USART_InitTypeDef usart         = {0};
    usart.USART_BaudRate            = baudRate;                       // 波特率
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制，不需要
    usart.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 模式，发送模式和接收模式均选择
    usart.USART_Parity              = USART_Parity_No;                // 奇偶校验，不需要
    usart.USART_StopBits            = USART_StopBits_1;               // 停止位，选择1位
    usart.USART_WordLength          = USART_WordLength_8b;            // 字长，选择8位
    USART_Init((USART_TypeDef *)srlList[idx].uart, &usart);

    /* 中断输出配置 */
    USART_ITConfig((USART_TypeDef *)srlList[idx].uart, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC配置 */
    NVIC_InitTypeDef nvic                  = {0};
    nvic.NVIC_IRQChannel                   = srlList[idx].irqn;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    nvic.NVIC_IRQChannelPreemptionPriority = PreemptingPriority(priority);
    nvic.NVIC_IRQChannelSubPriority        = SubPriority(priority);
    NVIC_Init(&nvic);

    /* USART使能 */
    USART_Cmd((USART_TypeDef *)srlList[idx].uart, ENABLE); // 使能USARTx，串口开始运行
}

/* ******************** 初始化函数 */

/*





*/

/* 发送函数 ******************** */

/******************************************************************
 * \brief      串口发送单字节
 * \param[in]  idx 串口索引
 * \param[in]  byte 要发送的字节
 */
void serial_send_byte(srl_e idx, u8 byte)
{
    USART_SendData(srlList[idx].uart, byte);
    while (USART_GetFlagStatus(srlList[idx].uart, USART_FLAG_TXE) == RESET);
}

/******************************************************************
 * \brief      串口发送字符串
 * \param[in]  idx 串口索引
 * \param[in]  str 要发送的字符串
 */
void serial_send_string(srl_e idx, u8 *str)
{
    u8 i;
    for (i = 0; str[i] != '\0'; i++)
        serial_send_byte(idx, str[i]);
}

/******************************************************************
 * \brief      串口格式化发送函数
 * \param[in]  idx 串口索引
 * \param[in]  format 格式化字符串
 * \param[in]  ...    可变参数列表
 */
void serial_printf(srl_e idx, u8 *format, ...)
{
    u8 str[SRL_SEND_LEN] = {0};
    va_list arg;
    va_start(arg, format);
    vsprintf((char *)str, (const char *)format, arg);
    va_end(arg);
    serial_send_string(idx, str);
}

/* ******************** 发送函数 */
