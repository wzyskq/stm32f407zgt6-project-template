#include "serial.h"

/* Global Macros ----------------------------------------------------------- */

// 详见 serial.h

/* Private Variables ------------------------------------------------------- */

// 私有常量数组（可自定义）

static const u32 srlRccUart[] = {0, RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4};
static const u32 srlRccGpio[] = {0, RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOD, RCC_AHB1Periph_GPIOA};

static GPIO_TypeDef *srlGpioPort[]  = {0, GPIOA, GPIOD, GPIOD, GPIOA};
static USART_TypeDef *srlUartPort[] = {0, USART1, USART2, USART3, UART4};
static const u8 srlUartIRQn[]       = {0, USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn};

static const u16 srlGpioPinTx[] = {0, GPIO_Pin_9, GPIO_Pin_5, GPIO_Pin_8, GPIO_Pin_0};
static const u16 srlGpioPinRx[] = {0, GPIO_Pin_10, GPIO_Pin_6, GPIO_Pin_9, GPIO_Pin_1};
static const u8 srlGpioAF[]     = {0, GPIO_AF_USART1, GPIO_AF_USART2, GPIO_AF_USART3, GPIO_AF_UART4};
static const u8 srlGpioSrcTx[]  = {0, GPIO_PinSource9, GPIO_PinSource5, GPIO_PinSource8, GPIO_PinSource0};
static const u8 srlGpioSrcRx[]  = {0, GPIO_PinSource10, GPIO_PinSource6, GPIO_PinSource9, GPIO_PinSource1};

/* Global Variables -------------------------------------------------------- */

u8 srlReFlag = 0; // 串口调试返回标志位

u8 serialTimeFlag = 0; // MCU 运行时间标志位
u16 serialTime    = 0; // 运行时间 单位：10ms

u8 spdLogFlag = 1; // 速度调试标志位（1：开启；0：关闭）

// 串口接收缓冲区（可自定义）

__IO u8 srlSigBuf[SRL_SIGBUF_LEN] = {0}; // 标志位缓存
__IO u8 srlSigFlg                 = 0;   // 标志位接收完成标志
__IO u8 srlPkgBuf[SRL_PKGBUF_LEN] = {0}; // 数据包缓存
__IO u8 srlPkgFlg                 = 0;   // 数据包接收完成标志
__IO u8 srlPidBuf[SRL_PIDBUF_LEN] = {0}; // PID 缓存
__IO u8 srlPidFlg                 = 0;   // PID 接收完成标志
__IO u8 srlCmdBuf[SRL_CMDBUF_LEN] = {0}; // 命令缓存
__IO u8 srlCmdFlg                 = 0;   // 命令接收完成标志

/* Global Functions -------------------------------------------------------- */

/* 发送函数 ******************** */

/******************************************************************
 * \brief      串口初始化
 * \param[in]  srlNum 串口号 x=1,2,3,4
 * \param[in]  baudRate 波特率
 * \param[in]  subPriority 响应优先级 x=0..3
 *
 * \note       - 暂时仅支持 USART1..4
 *             - 中断优先级：NVIC 分组1 (抢占1, 响应0~7)
 */
void serial_init(u8 srlNum, u32 baudRate, u8 subPriority)
{
    /* 参数检查 */
    if (srlNum < 1 || srlNum > 4) return;

    /* 开启时钟 */
    if (srlNum == 1 || srlNum == 6)
        RCC_APB2PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART1/6 在 APB2
    else
        RCC_APB1PeriphClockCmd(srlRccUart[srlNum], ENABLE); // USART2/3、UART4/5 在 APB1
    RCC_AHB1PeriphClockCmd(srlRccGpio[srlNum], ENABLE);     // GPIOx 在 AHB1

    /* GPIO 初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = srlGpioPinTx[srlNum] | srlGpioPinRx[srlNum]; // 选择引脚 Tx 和 Rx
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                                // 配置为复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                            // 速度 50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                               // 推挽输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                                // 上拉
    GPIO_Init((GPIO_TypeDef *)srlGpioPort[srlNum], &GPIO_InitStructure);

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
    USART_Init((USART_TypeDef *)srlUartPort[srlNum], &USART_InitStructure);

    /* 中断输出配置 */
    USART_ITConfig((USART_TypeDef *)srlUartPort[srlNum], USART_IT_RXNE, ENABLE); // 开启串口接收数据的中断

    /* NVIC配置 */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel                   = srlUartIRQn[srlNum]; // 选择配置 NVIC 的 USARTx 线
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;              // 指定 NVIC 线路使能
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;                   // 抢占优先级为 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = subPriority;         // 响应优先级为设定值
    NVIC_Init(&NVIC_InitStructure);

    /* USART使能 */
    USART_Cmd((USART_TypeDef *)srlUartPort[srlNum], ENABLE); // 使能USARTx，串口开始运行
}

/* ******************** 初始化函数 */

/*





*/

/* 发送函数 ******************** */

/******************************************************************
 * \brief      串口发送一个字节
 * \param[in]  srlNum 对应的串口号
 * \param[in]  Byte 要发送的字节
 */
void serial_send_byte(u8 srlNum, u8 Byte)
{
    USART_SendData((USART_TypeDef *)srlUartPort[srlNum], Byte);
    while (USART_GetFlagStatus((USART_TypeDef *)srlUartPort[srlNum], USART_FLAG_TXE) == RESET);
}

/******************************************************************
 * \brief      串口发送一个字符串
 * \param[in]  srlNum 对应的串口号
 * \param[in]  String 要发送的字符串
 */
void serial_send_string(u8 srlNum, u8 *String)
{
    u8 i;
    for (i = 0; String[i] != '\0'; i++)
        serial_send_byte(srlNum, String[i]);
}

/******************************************************************
 * \brief      自定义 printf 函数
 * \param[in]  srlNum 对应的串口号
 * \param[in]  format 格式化字符串
 * \param[in]  ...    可变参数列表
 */
void serial_printf(u8 srlNum, const char *format, ...)
{
    char String[SRL_PRINTF_LEN];
    va_list arg;
    va_start(arg, format);
    vsnprintf(String, SRL_PRINTF_LEN, format, arg);
    va_end(arg);
    serial_send_string(srlNum, (u8 *)String);
}

/* ******************** 发送函数 */

/*





*/

/* 处理函数 ******************** */

/******************************************************************
 * \brief  处理 USART3 接收到的数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void serial_decode_sign(void)
{
    if (!srlSigFlg)
        return;

    u8 t = srlSigBuf[1];
    // pass

    srlSigFlg = 0;
    if (srlReFlag)
        serial_printf(1, "3> Sign: %c\n", t);
}

/******************************************************************
 * \brief  处理 USART3 接收到的数据包内容
 * \note   在主循环中调用，当 srlPkgFlg 为 1 时
 */
void serial_decode_packet(void)
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
        serial_printf(1, "Diff: %d, %d\n", 80, diff);
}

/******************************************************************
 * \brief  处理 USART1 接收到的数据包内容
 * \note   在主循环中调用，当 srlPidFlg 为 1 时
 */
void serial_decode_pid(void)
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
        // case 'C':
        //     pidValue[num].kcoef = v;
        //     break;
        case 'P':
            pidValue[num].kp = v;
            break;
        case 'I':
            pidValue[num].ki = v;
            break;
        case 'D':
            pidValue[num].kd = v;
            break;
        default:
            break;
    }

    srlPidFlg = 0;
    if (srlReFlag)
        serial_printf(1, "1> %c %d %g\n", type, num, v);
}

/******************************************************************
 * \brief  解析 srlCmdBuf 数据包内容
 * \note   在主循环中调用，当 srlCmdFlg 为 1 时
 * \note   支持的命令：
 *            debug -i/-o              打开/关闭调试模式
 *            oled -d <x> <y> <str>    在 OLED 上显示字符串（大小默认为）
 *            srl -r <x> <str>         向串口x发送字符串
 *            led -i/-o/-t <x>         打开/关闭/切换 LEDx
 *            svo -p/-s <t> <c> <val>  设置指定时钟舵机位置/速度
 * \note   Q&A：
 *            Q1: 为什么要用传递指针 rCmd？
 *            A1: 若直接将 strmatch_s 的值赋给 cCmd，一旦第一个条件不满足，其值会直接变成 NULL，导致后续判断无法进行
 */
void serial_decode_cmd(void)
{
    if (!srlCmdFlg)
        return;

    u8 *cCmd = srlCmdBuf + 1; // 正文指针
    u8 *rCmd = NULL;          // 传递指针
    s16 arg  = 0;

    if (rCmd = strmatch_s(cCmd, "debug")) {
        if (strmatch_s(rCmd, "-i"))
            srlReFlag = 1;
        else if (strmatch_s(rCmd, "-o"))
            srlReFlag = 0;

    } else if (rCmd = strmatch_s(cCmd, "srl")) {
        if (cCmd = strmatch_s(rCmd, "-r")) {
            arg = (u8)strtof(cCmd, &cCmd);
            serial_printf(arg, "%s\r\n", cCmd + 1);
        }

    } else if (rCmd = strmatch_s(cCmd, "trk")) {
        if (cCmd = strmatch_s(rCmd, "--mode")) {
            trkMode   = (u8)strtof(cCmd, NULL);
            whlSpd[0] = whlSpd[1] = 0;
        } else if (cCmd = strmatch_s(rCmd, "--spd")) {
            trkSpd = (s16)strtof(cCmd, NULL);
        }

    } else if (rCmd = strmatch_s(cCmd, "dir")) {
        if (cCmd = strmatch_s(rCmd, "-s")) {
            s16 p = (s16)strtof(cCmd, &cCmd);
            // 限幅处理
            while (p <= -180) { p += 360; }
            while (p > 180) { p -= 360; }
            carDeg = p;
        }

    } else if (rCmd = strmatch_s(cCmd, "spd")) {
        if (cCmd = strmatch_s(rCmd, "-s")) {
            u8 t  = (u8)strtof(cCmd, &cCmd);  // 获取电机编号
            s16 p = (s16)strtof(cCmd, &cCmd); // 获取目标 PWM
            if (!t)
                whlSpd[0] = whlSpd[1] = p;
            else
                whlSpd[t - 1] = p;
        } else if (cCmd = strmatch_s(rCmd, "-l")) {
            if (strmatch_s(cCmd, "-i"))
                spdLogFlag = 1;
            else if (strmatch_s(cCmd, "-o"))
                spdLogFlag = 0;
        } else if (cCmd = strmatch_s(rCmd, "--div"))
            spdStepDiv = (u8)strtof(cCmd, NULL);
        else if (cCmd = strmatch_s(rCmd, "--err"))
            spdStepErr = (u8)strtof(cCmd, NULL);
        else if (cCmd = strmatch_s(rCmd, "--exp"))
            spdStepExp = (u8)strtof(cCmd, NULL);

    } else if (rCmd = strmatch_s(cCmd, "sm")) {
        if (cCmd = strmatch_s(rCmd, "-v")) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            s32 vel = (s32)strtof(cCmd, &cCmd);
            u8 dir  = (vel > 0) ? 0 : 1; // 根据速度正负自动判断方向
            u8 acc  = (u8)strtof(cCmd, &cCmd);
            vel *= 10;        // 开启了缩小十倍，实际输入单位为 0.1 RPM
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Vel_Control(4, addr, dir, (u16)ABS(vel), acc, false);
        } else if (cCmd = strmatch_s(rCmd, "-p")) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            s32 vel = (s32)strtof(cCmd, &cCmd);
            u8 acc  = (u8)strtof(cCmd, &cCmd);
            s32 pos = (s32)strtof(cCmd, &cCmd);
            vel *= 10;                  // 开启了缩小十倍，实际输入单位为 0.1 RPM
            u8 dir = (pos > 0) ? 0 : 1; // 根据位置正负自动判断方向
            // while (zdtTvFlg);           // 等待清零
            Emm_V5_Pos_Control(4, addr, dir, (u16)ABS(vel), acc, (u32)ABS(pos), true, false);
        } else if (cCmd = strmatch_s(rCmd, "-o"))
        {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            while (zdtTvFlg); // 等待清零
            Emm_V5_Stop_Now(4, addr, false);
        }
        

    } else if (rCmd = strmatch_s(cCmd, "oled")) {
        // if (cCmd = strmatch_s(rCmd, "-d")) {
        //     u8 x = (u8)strtof(cCmd, &cCmd);
        //     u8 y = (u8)strtof(cCmd, &cCmd);
        //     oled_printf(x, y, OLED_8X16, "%s", cCmd + 1);
        // } else
        if (cCmd = strmatch_s(rCmd, "-v")) {
            oledViewIdx = (u8)strtof(cCmd, NULL);
        }

    } else if (rCmd = strmatch_s(cCmd, "led")) {
        // if (cCmd = strmatch_s(rCmd, "-i")) {
        //     arg = strtof(cCmd, NULL);
        //     led_on(arg);
        // } else if (cCmd = strmatch_s(rCmd, "-o")) {
        //     arg = strtof(cCmd, NULL);
        //     led_off(arg);
        // } else if (cCmd = strmatch_s(rCmd, "-t")) {
        //     arg = strtof(cCmd, NULL);
        //     led_toggle(arg);
        // }

    } else if (rCmd = strmatch_s(cCmd, "svo")) {
        // u8 t = 0, c = 0;
        // if (cCmd = strmatch_s(rCmd, "-p")) {
        //     t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
        //     c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
        //     arg = strtof(cCmd, NULL);      // 获取位置
        //     servo_set_pos(t, c, arg);      // 设置舵机位置
        // } else if (cCmd = strmatch_s(rCmd, "-s")) {
        //     t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
        //     c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
        //     arg = strtof(cCmd, NULL);      // 获取速度
        //     servo_set_spd(t, c, arg);      // 设置舵机速度
        // } else if (cCmd = strmatch_s(rCmd, "-d")) {
        //     t   = (u8)strtof(cCmd, &cCmd);   // 获取舵机时钟
        //     c   = (u8)strtof(cCmd, &cCmd);   // 获取舵机通道
        //     arg = strtof(cCmd, NULL);        // 获取占空比
        //     timer_pwmOut_setDuty(t, c, arg); // 直接设置占空比
        // }

    } else if (srlReFlag) {
        serial_printf(1, "> Unknown CMD\n");
    }

    srlCmdFlg = 0;
    if (srlReFlag)
        serial_printf(1, "> CMD: %s\n", srlCmdBuf + 1);
}

/* ******************** 处理函数 */

/*





*/

/* 等待函数 ******************** */

/******************************************************************
 * \brief  while 型串口等待机
 * \param  flagString* 要发送的标志字符串
 * \param  getFlagFun* 获取标志位的函数指针
 *
 * \note 该函数用于阻塞等待，直到获取到标志位为止
 */
void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void))
{
    serial_send_string(3, flagString);
    serialTimeFlag = 1;
    serialTime     = 0;
    while (!getFlagFun()) {
        serial_decode_packet();
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
            serialTime = 0;
            serial_send_string(3, flagString);
        }
    }
    serialTimeFlag = 0;
    serialTime     = 0; // 清除计时器
}

/******************************************************************
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
        serial_send_string(3, flagString);
        serialTimeFlag = 1;
        return 0; // 由于串口解析是在主循环中进行的，这里不阻塞等待
    }

    if (!getFlagFun()) {
        // serial_decode_packet(); // 非阻塞等待，主循环会处理数据包
        if (serialTimeFlag && serialTime > SERIAL_TIMEOUT) {
            serialTime = 1; // 从 1 开始计时，防止 serialTime 为 0 时直接发送
            serial_send_string(3, flagString);
        }
        return 0; // 获取失败
    }

    serialTimeFlag = 0;
    serialTime     = 0;
    return 1; // 获取成功
}

/* ******************** 等待函数 */
