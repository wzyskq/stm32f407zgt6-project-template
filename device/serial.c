#include "serial.h"

/* Global Macros ----------------------------------------------------------- */

// 详见 serial.h

/* Private Variables ------------------------------------------------------- */

// 私有常量数组（可自定义）

static const u32 srlRccUart[] = {0, RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3};
static const u32 srlRccGpio[] = {0, RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOD};

static const GPIO_TypeDef *srlGpioPort[]  = {0, GPIOA, GPIOD, GPIOD};
static const USART_TypeDef *srlUartPort[] = {0, USART1, USART2, USART3};
static const enum IRQn srlUartIRQn[]      = {0, USART1_IRQn, USART2_IRQn, USART3_IRQn};

static const u16 srlGpioPinTx[] = {0, GPIO_Pin_9, GPIO_Pin_5, GPIO_Pin_8};
static const u16 srlGpioPinRx[] = {0, GPIO_Pin_10, GPIO_Pin_6, GPIO_Pin_9};
static const u8 srlGpioAF[]     = {0, GPIO_AF_USART1, GPIO_AF_USART2, GPIO_AF_USART3};
static const u8 srlGpioSrcTx[]  = {0, GPIO_PinSource9, GPIO_PinSource5, GPIO_PinSource8};
static const u8 srlGpioSrcRx[]  = {0, GPIO_PinSource10, GPIO_PinSource6, GPIO_PinSource9};

/* Global Variables -------------------------------------------------------- */

u8 srlReFlag = 0; // 串口调试返回标志位

// 串口接收缓冲区（可自定义）

volatile u8 srlSigBuf[SRL_SIGBUF_LEN] = {0}; // 标志位缓存
volatile u8 srlSigFlg                 = 0;   // 标志位接收完成标志
volatile u8 srlPkgBuf[SRL_PKGBUF_LEN] = {0}; // 数据包缓存
volatile u8 srlPkgFlg                 = 0;   // 数据包接收完成标志
volatile u8 srlPidBuf[SRL_PIDBUF_LEN] = {0}; // PID 缓存
volatile u8 srlPidFlg                 = 0;   // PID 接收完成标志
volatile u8 srlCmdBuf[SRL_CMDBUF_LEN] = {0}; // 命令缓存
volatile u8 srlCmdFlg                 = 0;   // 命令接收完成标志

/* Global Functions -------------------------------------------------------- */

/* 发送函数 ******************** */

/**************************************************************
 * \brief  串口初始化
 * \param  srlNum 串口号 x=1,2,3
 * \param  baudRate 波特率
 * \param  subPriority 响应优先级 x=0~3
 *
 * \note   - 暂时仅支持 USART1~3
 *         - 中断优先级：NVIC 分组1 (抢占1, 响应0~7)
 */
void serial_init(u8 srlNum, u32 baudRate, u8 subPriority)
{
    /* 参数检查 */
    if (srlNum < 1 || srlNum > 3) return;

    /* 开启时钟 */
    if (srlNum == 1 || srlNum == 6)
        RCC_APB2PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART1/6 在 APB2
    else
        RCC_APB1PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART2/3、UART4/5 在 APB1
    RCC_AHB1PeriphClockCmd(srlRccGpio[srlNum], ENABLE);     // GPIOx 在 AHB1

    /* GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = srlGpioPinTx[srlNum] | srlGpioPinRx[srlNum];
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                        // 配置为复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                    // 速度 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                       // 推挽输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                        // 上拉
    GPIO_Init((GPIO_TypeDef *)srlGpioPort[srlNum], &GPIO_InitStructure); // 初始化 GPIOx

    /* 连接引脚USARTx */
    GPIO_PinAFConfig((GPIO_TypeDef *)srlGpioPort[srlNum], srlGpioSrcTx[srlNum], srlGpioAF[srlNum]);
    GPIO_PinAFConfig((GPIO_TypeDef *)srlGpioPort[srlNum], srlGpioSrcRx[srlNum], srlGpioAF[srlNum]);

    /* USART初始化 */
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = baudRate;                       // 波特率
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 硬件流控制，不需要
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;  // 模式，发送模式和接收模式均选择
    USART_InitStructure.USART_Parity              = USART_Parity_No;                // 奇偶校验，不需要
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;               // 停止位，选择1位
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;            // 字长，选择8位
    USART_Init((USART_TypeDef *)srlUartPort[srlNum], &USART_InitStructure);         // 将结构体变量交给USART_Init，配置USARTx

    /* 中断输出配置 */
    USART_ITConfig((USART_TypeDef *)srlUartPort[srlNum], USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 配置NVIC为分组1 (抢占0~1，响应0~7)

    /*N VIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;                                        // 定义结构体变量
    NVIC_InitStructure.NVIC_IRQChannel                   = srlUartIRQn[srlNum]; // 选择配置NVIC的USART3线
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;              // 指定NVIC线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;                   // 抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = subPriority;         // 响应优先级为设定值
    NVIC_Init(&NVIC_InitStructure);                                             // 将结构体变量交给NVIC_Init，配置NVIC外设

    /* USART使能 */
    USART_Cmd((USART_TypeDef *)srlUartPort[srlNum], ENABLE); // 使能USARTx，串口开始运行
}

/* ******************** 初始化函数 */

/*





*/

/* 发送函数 ******************** */

/**
 * \brief  串口发送一个字节
 * \param  USARTx 对应的串口号
 * \param  Byte 要发送的字节
 */
void serial_send_byte(USART_TypeDef *USARTx, u8 Byte)
{
    USART_SendData(USARTx, Byte);
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/**
 * \brief  串口发送一个字符串
 * \param  USARTx 对应的串口号
 * \param  String 要发送的字符串
 */
void serial_send_string(USART_TypeDef *USARTx, u8 *String)
{
    u8 i;
    for (i = 0; String[i] != '\0'; i++)
        serial_send_byte(USARTx, String[i]);
}

/**
 * \brief  自定义 printf 函数
 * \param  USARTx 对应的串口号
 * \param  format 格式化字符串
 * \param  ...    可变参数列表
 */
void serial_printf(USART_TypeDef *USARTx, u8 *format, ...)
{
    char String[128] = {0};
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    serial_send_string(USARTx, String);
}

/* ******************** 发送函数 */

/*





*/

/* 处理函数 ******************** */

/**
 * \brief  处理 USART3 接收到的数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void serial_process_sign(void)
{
    if (!srlSigFlg)
        return;

    u8 t = srlSigBuf[1];
    // pass

    srlSigFlg = 0;
    if (srlReFlag)
        serial_printf(USART1, "3> Sign: %c\n", t);
}

/**
 * \brief  处理 USART3 接收到的数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void serial_process_packet(void)
{
    if (!srlPkgFlg)
        return;

    s16 diff = (srlPkgBuf[1] << 8) | srlPkgBuf[2];

    // 限幅
    diff = (diff > 160) ? 160 : diff;
    diff = (diff < 0) ? 0 : diff;

    // 更新 PID 误差

    srlPkgFlg = 0;
    if (srlReFlag)
        serial_printf(USART1, "Diff: %d, %d\n", 80, diff);
}

/**
 * \brief  处理 USART1 接收到的数据包内容
 * \note   在主循环中调用，当 srlPidFlg 为 1 时
 */
void serial_process_pid(void)
{
    if (!srlPidFlg)
        return;

    u8 len = srlPidBuf[0];
    if (len < 4) { // 至少要有一个字节
        srlPidFlg = 0;
        return;
    }

    // 数据实际从 srlPidBuf[1] 开始
    char type = srlPidBuf[1];
    u8 num    = srlPidBuf[2] - '0';
    float v   = strtof((char *)&srlPidBuf[4], NULL); // 从第4个字节开始转换为浮点数

    // e.g. {Pn 0.05}

    switch (type) {
        case 'P':
            pidValue[num].Kp = v;
            break;
        case 'I':
            pidValue[num].Ki = v;
            break;
        case 'D':
            pidValue[num].Kd = v;
            break;
        default:
            break;
    }
    srlPidFlg = 0;
    if (srlReFlag)
        serial_printf(USART1, "1> %c %d %g\n", type, num, v);
}

/**
 * \brief  处理 USART1 接收到的目标坐标数据包
 * \note   在主循环中调用，当 srlCmdFlg 为 3 时
 *         支持命令：
 *            (spd lll rrr) 设置左右轮目标速度
 *            (auto n)      设置巡线开关，n=0 关闭，n=1 开启
 *            (apul n)      设置巡线平均脉冲，n=脉冲值
 *            (lfds n)      设置巡线方向，n=-1 左转【逆时针】，n=0 直线，n=1 右转【顺时针】
 *            (keys n)      设置按键
 *            (vofa)        切换数据引擎
 */
void serial_process_cmd(void)
{
    if (!srlCmdFlg)
        return;

    if (srlCmdBuf[1] == 's') {
        u8 *pEnd;
        s16 l = (s16)strtof((char *)&srlCmdBuf[5], &pEnd);
        s16 r = (s16)strtof(pEnd + 1, NULL); // strof() 会主动跳过空格，+1 可以省略
    } else if (srlCmdBuf[1] == 'a') {
        s16 n = (s16)strtof((char *)&srlCmdBuf[6], NULL);
    } else if (srlCmdBuf[1] == 'l') {
        s8 n = (s8)strtof((char *)&srlCmdBuf[6], NULL);
    } else if (srlCmdBuf[1] == 'k') {
        u8 n = (u8)strtof((char *)&srlCmdBuf[6], NULL);
    } else if (srlCmdBuf[1] == 'v') {
    }

    srlCmdFlg = 0;
    if (srlReFlag)
        serial_printf(USART1, "1> CMD: %s\n", &srlCmdBuf[1]);
}

/* ******************** 处理函数 */

/*





*/

/* 等待函数 ******************** */

/**
 * \brief  while 型串口等待机
 * \param  flagString* 要发送的标志字符串
 * \param  getFlagFun* 获取标志位的函数指针
 *
 * \note 该函数用于阻塞等待，直到获取到标志位为止
 */
void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void))
{
    serial_send_string(USART3, flagString);
    serialTimeFlag = 1;
    serialTime     = 0;
    while (!getFlagFun()) {
        serial_process_packet();
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
            serialTime = 0;
            serial_send_string(USART3, flagString);
        }
    }
    serialTimeFlag = 0;
    serialTime     = 0; // 清除计时器
}

/**
 * \brief  if 型串口等待机
 * \param  flagString* 要发送的标志字符串
 * \param  getFlagFun* 获取标志位的函数指针
 * \retval 0 获取失败，1 获取成功
 *
 * \note 该函数用于非阻塞等待，适用于需要在主循环中处理数据包的场景
 */
u8 serial_wait_if(u8 *flagString, u8 (*getFlagFun)(void))
{
    if (!serialTimeFlag) // 如果 serialTimeFlag 为 0，表示首次调用
    {
        serial_send_string(USART3, flagString);
        serialTimeFlag = 1;
        return 0; // 由于串口解析是在主循环中进行的，这里不阻塞等待
    }

    if (!getFlagFun()) {
        // serial_process_packet(); // 非阻塞等待，主循环会处理数据包
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
            serialTime = 1; // 从 1 开始计时，防止 serialTime 为 0 时直接发送
            serial_send_string(USART3, flagString);
        }
        return 0; // 获取失败
    }

    serialTimeFlag = 0;
    serialTime     = 0;
    return 1; // 获取成功
}

/* ******************** 等待函数 */
