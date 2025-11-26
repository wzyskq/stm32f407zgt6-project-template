#include "main.h"

volatile BYTE serialSignBuf[SERIAL_BUF_SIZE];
volatile BYTE serialSignOk = 0;
volatile BYTE serialPacketBuf[SERIAL_BUF_SIZE];
volatile BYTE serialPacketOk = 0;
volatile BYTE serialPIDBuf[SERIAL_BUFFER_SIZE];
volatile BYTE serialPIDOk = 0;
volatile BYTE serialCMDBuf[SERIAL_BUFFER_SIZE];
volatile BYTE serialCMDOk = 0;

BYTE signXFlag = 0;
BYTE signDFlag = 0;

BYTE get_sign_x_flag(void)
{
    if (signXFlag)
    {
        signXFlag = 0;
        return 1; // 返回1表示有新标志
    }
    return 0; // 返回0表示没有新标志
}

BYTE get_sign_d_flag(void)
{
    if (signDFlag)
    {
        signDFlag = 0;
        return 1; // 返回1表示有新标志
    }
    return 0; // 返回0表示没有新标志
}

/**
 * @brief  串口初始化 PD8 PD9 (USART3)
 * @param  BaudRate 波特率
 */
void serial_init_3(UINT BaudRate)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // 开启USART3的时钟 (APB1总线)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);  // 开启GPIOD的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*连接引脚到USART3*/
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

    /*USART初始化*/
    USART_InitTypeDef USART_InitStructure;                                          // 定义结构体变量
    USART_InitStructure.USART_BaudRate = BaudRate;                                  // 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制，不需要
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                 // 模式，发送模式和接收模式均选择
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 奇偶校验，不需要
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位，选择1位
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长，选择8位
    USART_Init(USART3, &USART_InitStructure);                                       // 将结构体变量交给USART_Init，配置USART3

    /*中断输出配置*/
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /*NVIC中断分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC为分组2

    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;                      // 定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // 选择配置NVIC的USART3线
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应优先级为1
    NVIC_Init(&NVIC_InitStructure);                           // 将结构体变量交给NVIC_Init，配置NVIC外设

    /*USART使能*/
    USART_Cmd(USART3, ENABLE); // 使能USART3，串口开始运行
}

/**
 * @brief  串口初始化 PD5 PD6 (USART2)
 * @param  BaudRate 波特率
 */
void serial_init_2(UINT BaudRate)
{
    /* 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* 连接引脚到USART2 */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);

    /* 中断输出配置 */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* NVIC中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        // 响应优先级为 2
    NVIC_Init(&NVIC_InitStructure);

    /* USART使能 */
    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  串口初始化 PA9 PA10 (USART1)
 * @param  BaudRate 波特率
 */
void serial_init_1(UINT BaudRate)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*连接引脚到USART1*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    /*USART初始化*/
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = BaudRate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    /*中断输出配置*/
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /*NVIC中断分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 配置NVIC为分组2

    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级为 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 响应优先级为 3
    NVIC_Init(&NVIC_InitStructure);

    /*USART使能*/
    USART_Cmd(USART1, ENABLE); // 使能USART1，串口开始运行
}

/*







*/

/**
 * @brief  串口发送一个字节
 * @param  USARTx 对应的串口号
 * @param  Byte 要发送的字节
 */
void serial_send_byte(USART_TypeDef *USARTx, BYTE Byte)
{
    USART_SendData(USARTx, Byte);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
        ;
}

/**
 * @brief  串口发送一个字符串
 * @param  USARTx 对应的串口号
 * @param  String 要发送的字符串
 */
void serial_send_string(USART_TypeDef *USARTx, BYTE *String)
{
    BYTE i;
    for (i = 0; String[i] != '\0'; i++)
        serial_send_byte(USARTx, String[i]);
}

/**
 * @brief  自定义 printf 函数
 * @param  USARTx 对应的串口号
 * @param  format 格式化字符串
 * @param  ...    可变参数列表
 */
void serial_printf(USART_TypeDef *USARTx, BYTE *format, ...)
{
    char String[128];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    serial_send_string(USARTx, String);
}

/*







*/

/**
 * @brief  处理 USART3 接收到的数据包内容
 * @note   在主循环中调用，当 serialPacketOk 为 1 时
 */
void serial_process_sign(void)
{
    if (!serialSignOk)
        return;

    BYTE t = serialSignBuf[1];

    // pass

    serialSignOk = 0;
    serial_printf(USART1, "3> Sign: %c\n", t);
}

/**
 * @brief  处理 USART3 接收到的数据包内容
 * @note   在主循环中调用，当 serialPacketOk 为 1 时
 */
void serial_process_packet(void)
{
    if (!serialPacketOk)
        return;

    BYTE t = serialSignBuf[1];

    // pass

    serialPacketOk = 0;
    serial_printf(USART1, "3> Pack: %d\n", t);
}

/**
 * @brief  处理 USART1 接收到的数据包内容
 * @note   在主循环中调用，当 serialPIDOk 为 1 时
 */
void serial_process_pid(void)
{
    if (!serialPIDOk)
        return;

    BYTE len = serialPIDBuf[0];
    if (len < 4)
    { // 至少要有一个字节
        serialPIDOk = 0;
        return;
    }

    // 数据实际从 serialPIDBuf[1] 开始
    char type = serialPIDBuf[1];
    BYTE num = serialPIDBuf[2] - '0';
    float v = strtof((char *)&serialPIDBuf[4], NULL); // 从第4个字节开始转换为浮点数
    char dis[8];

    // e.g. {Pn 0.05}

    switch (type)
    {
    case 'P':
        pidValue[num].Kp = v;
        // sprintf(dis, "%g", pidValue[num].Kp);
        // OLED_ShowString(1, 1, "      ");
        // OLED_ShowString(1, 1, dis);
        break;
    case 'I':
        pidValue[num].Ki = v;
        // sprintf(dis, "%g", pidValue[num].Ki);
        // OLED_ShowString(2, 1, "      ");
        // OLED_ShowString(2, 1, dis);
        break;
    case 'D':
        pidValue[num].Kd = v;
        // sprintf(dis, "%g", pidValue[num].Kd);
        // OLED_ShowString(3, 1, "      ");
        // OLED_ShowString(3, 1, dis);
        break;
    default:
        break;
    }
    serialPIDOk = 0;
    serial_printf(USART1, "1> %c %d %g\n", type, num, v);
}

/**
 * @brief  处理 USART1 接收到的目标坐标数据包
 * @note   在主循环中调用，当 serialCMDOk 为 3 时
 */
void serial_process_cmd(void)
{
    if (!serialCMDOk)
        return;

    BYTE t = serialCMDBuf[1];

    serialCMDOk = 0;
    serial_printf(USART1, "1> CMD: %c\n", t);
}

/*







*/

/**
 * @brief  while 型串口等待机
 * @param  flagString* 要发送的标志字符串
 * @param  getFlagFun* 获取标志位的函数指针
 *
 * @note 该函数用于阻塞等待，直到获取到标志位为止
 */
void serial_wait_while(BYTE *flagString, BYTE (*getFlagFun)(void))
{
    serial_send_string(USART3, flagString);
    serialTimeFlag = 1;
    serialTime = 0;
    while (!getFlagFun())
    {
        serial_process_packet();
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT)
        {
            serialTime = 0;
            serial_send_string(USART3, flagString);
        }
    }
    serialTimeFlag = 0;
    serialTime = 0; // 清除计时器
}

/**
 * @brief  if 型串口等待机
 * @param  flagString* 要发送的标志字符串
 * @param  getFlagFun* 获取标志位的函数指针
 * @retval 0 获取失败，1 获取成功
 *
 * @note 该函数用于非阻塞等待，适用于需要在主循环中处理数据包的场景
 */
BYTE serial_wait_if(BYTE *flagString, BYTE (*getFlagFun)(void))
{
    if (!serialTimeFlag) // 如果 serialTimeFlag 为 0，表示首次调用
    {
        serial_send_string(USART3, flagString);
        serialTimeFlag = 1;
        return 0; // 由于串口解析是在主循环中进行的，这里不阻塞等待
    }

    if (!getFlagFun())
    {
        // serial_process_packet(); // 非阻塞等待，主循环会处理数据包
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT)
        {
            serialTime = 1; // 从 1 开始计时，防止 serialTime 为 0 时直接发送
            serial_send_string(USART3, flagString);
        }
        return 0; // 获取失败
    }

    serialTimeFlag = 0;
    serialTime = 0;
    return 1; // 获取成功
}

/*







*/
