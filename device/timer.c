#include "timer.h"

/* Private Macros ---------------------------------------------------------- */

/* Private variables ------------------------------------------------------- */

/******************************************************************
 * \brief    定时器编号映射表
 * \note     由于当前芯片引脚、定时器很多，故不全部列出。请根据项目需要选择定时器，添加相关配置并完善以下映射表
 * \extends   - STM32F407 定时器编号: 1~14
 * \example  依次使用了 TIM14、TIM1，则 timMapping 应定义为 {0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}
 * \warning  timMapping[0] = 0，有效值从 timMapping[1] 开始
 */
static const u8 timMapping[15] = {0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
// 位序参照                       {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4}

/******************************************************************
 * \brief    定时器 RCC 外设时钟使能映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     仅需列出常用定时器的 RCC 外设时钟使能宏定义
 * \extends   - APB1 总线: TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
 *            - APB2 总线: TIM1, TIM8, TIM9, TIM10, TIM11
 * \example  RCC_APB2Periph_TIM1, RCC_APB1Periph_TIM2, etc.
 * \warning  timRccTim[0] = 0，有效值从 timRccTim[1] 开始
 */
static const u32 timRccTim[] = {
    0,
    RCC_APB1Periph_TIM7,
    RCC_APB2Periph_TIM1,
};

/******************************************************************
 * \brief    GPIO 外设时钟使能映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     仅需列出常用定时器的 GPIO 外设时钟使能宏定义
 * \extends   - AHB1 总线: GPIOA, GPIOB, GPIOC, GPIOD, GPIOE
 * \example  RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, etc.
 * \warning  timRccGpio[0] = 0，有效值从 timRccGpio[1] 开始
 */
static const u32 timRccGpio[] = {
    0,
    0,
    RCC_AHB1Periph_GPIOE,
};

/******************************************************************
 * \brief    定时器 GPIO 端口及引脚映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     每个定时器最多支持 4 路通道输出，未使用的通道以 0 填充
 * \extends   - GPIO 端口类型: GPIOA, GPIOB, GPIOC, GPIOD, GPIOE
 *            - 具体参照下方 timGpioPin 映射表注释 extends 部分
 * \example  {GPIOA, GPIOA, GPIOA, GPIOA}, etc.
 *           代表该定时器的 4 路通道均映射到 GPIOA 端口
 * \warning  timGpioPort[0] = 0，有效值从 timGpioPort[1] 开始
 */
static GPIO_TypeDef *timGpioPort[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, GPIOE, GPIOE},
};

/******************************************************************
 * \brief    定时器端口映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     仅需列出常用定时器的端口定义
 * \extends   - TIMx, x=1..14
 * \example  TIM1, TIM2, etc.
 * \warning  timTimePort[0] = 0，有效值从 timTimePort[1] 开始
 */
static TIM_TypeDef *timTimePort[] = {
    0,
    TIM7,
    TIM1,
};

/******************************************************************
 * \brief    定时器中断号映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note      - 仅需列出常用定时器的中断号宏定义；
 *            - 部分定时器可能共用一个中断号，请根据实际需求选择
 * \extends   - TIMx_IRQn, x=1..14
 *            - TIM1：更新/CCx/触发/刹车  TIM1_UP_TIM10_IRQn/TIM1_CC_IRQn/TIM1_TRG_COM_IRQn/TIM1_BRK_TIM9_IRQn
 *            - TIM2：更新/CCx/触发       TIM2_IRQn
 *            - TIM3：更新/CCx/触发       TIM3_IRQn
 *            - TIM4：更新/CCx/触发       TIM4_IRQn
 *            - TIM5：更新/CCx/触发       TIM5_IRQn
 *            - TIM6：更新               TIM6_DAC_IRQn
 *            - TIM7：更新               TIM7_IRQn
 *            - TIM8：更新/CCx/触发/刹车  TIM8_UP_TIM13_IRQn/TIM8_CC_IRQn/TIM8_TRG_COM_IRQn/TIM8_BRK_TIM12_IRQn
 *            - TIM9：更新/CCx           TIM1_BRK_TIM9_IRQn
 *            - TIM10：更新/CCx          TIM1_UP_TIM10_IRQn
 *            - TIM11：更新/CCx          TIM1_TRG_COM_IRQn
 *            - TIM12：更新/CCx          TIM8_BRK_TIM12_IRQn
 *            - TIM13：更新/CCx          TIM8_UP_TIM13_IRQn
 *            - TIM14：更新/CCx          TIM8_TRG_COM_IRQn
 * \example  TIM1_UP_TIM10_IRQn, TIM2_IRQn, etc.
 * \warning  timTimeIRQn[0] = 0，有效值从 timTimeIRQn[1] 开始
 */
static const u8 timTimeIRQn[] = {
    0,
    TIM7_IRQn,
    TIM1_CC_IRQn,
};

/******************************************************************
 * \brief    定时器 GPIO 引脚映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     每个定时器最多支持 4 路通道输出，未使用的通道以 0 填充
 * \extends   = CHx   CH1           CH2          CH3        CH4
 *            - TIM1: PA8/PE9,      PA9/PE11,    PA10/PE13, PA11/PE14
 *            - TIM2: PA0/PA5/PA15, PA1/PB3,     PA2/PB10,  PA3/PB11
 *            - TIM3: PA6/PB4/PC6,  PA7/PB5/PC7, PB0/PC8,   PB1/PC9
 *            - TIM4: PB6/PD12,     PB7/PD13,    PB8/PD14,  PB9/PD15
 *            - TIM5: PA0,          PA1,         PA2,       PA3
 *            - TIM6:
 *            - TIM7:
 *            - TIM8: PC6,          PC7,         PC8,       PC9
 *            - TIM9: PA2/PE5,      PA3/PE6
 *            - TIM10: PB8/PF6
 *            - TIM11: PB9/PF7
 *            - TIM12: PB14,        PB15
 *            - TIM13: PA6/PF8
 *            - TIM14: PA7/PF9
 *
 *            = CHxN  CH1N          CH2N           CH3N
 *            - TIM1: PA7/PB13/PE8, PB0/PB14/PE10, PB1/PB15/PE12
 *            - TIM8: PA7/PC7,      PB0/PC8,       PB1/PC9
 *
 *            = ETR
 *            - TIM1: PA12, PE7
 *            - TIM2: PA0, PA15
 *            - TIM3: PD2
 *            - TIM4: PE0
 *
 *            = BKIN
 *            - TIM1: PA6, PB12, PE15
 *            - TIM8: PA6
 *
 * \example  {GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11}, etc.
 *           代表该定时器的 4 路通道分别映射到对应引脚
 * \warning  timGpioPin[0] = 0，有效值从 timGpioPin[1] 开始
 */
static const u16 timGpioPin[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, GPIO_Pin_13, GPIO_Pin_14},
};

/******************************************************************
 * \brief    定时器 GPIO 复用功能映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     仅需列出常用定时器的 GPIO 复用功能宏定义
 * \extends   - GPIO_AF_TIMx, x=1..14
 * \example  GPIO_AF_TIM1, GPIO_AF_TIM2, etc.
 * \warning  timGpioAF[0] = 0，有效值从 timGpioAF[1] 开始
 */
static const u8 timGpioAF[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, GPIO_AF_TIM1, GPIO_AF_TIM1},
};

/******************************************************************
 * \brief    定时器 GPIO 引脚源映射表
 * \pre      修改索引对应 timMapping 表中的定时器编号
 * \note     每个定时器最多支持 4 路通道输出，未使用的通道以 0 填充
 * \extends   - GPIO_PinSourcex, x=0..15
 * \example  {GPIO_PinSource8, GPIO_PinSource9, GPIO_PinSource10, GPIO_PinSource11}, etc.
 *           代表该定时器的 4 路通道分别映射到对应引脚源
 * \warning  timGpioSrc[0] = 0，有效值从 timGpioSrc[1] 开始
 */
static const u8 timGpioSrc[][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, GPIO_PinSource13, GPIO_PinSource14},
};

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/**
 * \brief      定时器初始化
 * \param[in]  mode 定时器工作模式
 *   \arg       - timer: 基本定时器模式
 *   \arg       - pwmOut: PWM 输出模式
 *   \arg       - encoder: 编码器解码模式
 * \param[in]  timNum 定时器编号 1..14
 * \param[in]  chNum 通道编号，若为定时器模式则填 0；
 *                    - 支持多通道无序输入，如：通道一和三：13，通道一、四和二：142
 *                    - 编码器模式下仅支持双通道输入
 * \param[in]  arr 自动重装载值 + 1
 * \param[in]  psc 预分频值 + 1
 * \param[in]  subPriority 中断响应优先级 0~3
 * \note        - 本函数完成定时器基本功能初始化及 PWM 输出、编码器解码模式配置
 * \warning     - 确保私有变量配置正确！
 *              - timNum 必须在 timMapping 映射表中有对应配置!
 *              - 注意总线主频！
 */
void timer_init(timMode_t mode, u8 timNum, u16 chNum, u16 arr, u16 psc, u8 subPriority)
{
    /* 定时器检测 */
    if (timNum < 1 || timNum > 14) return;

    /* 开启时钟 */
    if (timNum == 1 || (timNum >= 8 && timNum <= 11)) {
        timNum = timMapping[timNum]; // 定时器重映射
        RCC_APB2PeriphClockCmd(timRccTim[timNum], ENABLE);
    } else {
        timNum = timMapping[timNum]; // 定时器重映射
        RCC_APB1PeriphClockCmd(timRccTim[timNum], ENABLE);
    }

    /* 使用内部时钟 */
    TIM_InternalClockConfig(timTimePort[timNum]);

    /* TIM 基本初始化 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_TimeBaseInitStructure.TIM_Period              = arr - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler           = psc - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision       = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode         = TIM_CounterMode_Up;
    TIM_TimeBaseInit(timTimePort[timNum], &TIM_TimeBaseInitStructure);

    /* 配置中断 */
    TIM_ClearFlag(timTimePort[timNum], TIM_FLAG_Update);
    TIM_ITConfig(timTimePort[timNum], TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure                  = {0};
    NVIC_InitStructure.NVIC_IRQChannel                   = timTimeIRQn[timNum];
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = subPriority;
    NVIC_Init(&NVIC_InitStructure);

    /* 解析通道号 */
    u8 chList[5] = {0};
    while (chNum) {
        u8 digit = chNum % 10;
        if (digit >= 1 && digit <= 4)
            chList[++chList[0]] = digit;
        chNum /= 10;
    }

    /* GPIO 初始化（适用于 pwmOut 和 encoder 模式） */
    if (mode == pwmOut || mode == encoder) {
        RCC_AHB1PeriphClockCmd(timRccGpio[timNum], ENABLE);

        GPIO_InitTypeDef GPIO_InitStructure = {0};
        GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType       = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd        = GPIO_PuPd_UP;

        for (u8 i = 1; i <= chList[0]; i++) {
            GPIO_PinAFConfig(timGpioPort[timNum][chList[i] - 1],
                             timGpioSrc[timNum][chList[i] - 1],
                             timGpioAF[timNum][chList[i] - 1]);

            GPIO_InitStructure.GPIO_Pin = timGpioPin[timNum][chList[i] - 1];
            GPIO_Init(timGpioPort[timNum][chList[i] - 1], &GPIO_InitStructure);
        }
    }

    /* PWM 输出模式 */
    if (mode == pwmOut) {
        TIM_OCInitTypeDef TIM_OCInitStructure = {0};
        TIM_OCInitStructure.TIM_OCMode        = TIM_OCMode_PWM1;
        TIM_OCInitStructure.TIM_OCPolarity    = TIM_OCPolarity_High;
        TIM_OCInitStructure.TIM_OutputState   = TIM_OutputState_Enable;
        TIM_OCInitStructure.TIM_Pulse         = 0;
        TIM_OCStructInit(&TIM_OCInitStructure);

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                TIM_OC1Init(timTimePort[timNum], &TIM_OCInitStructure);
                TIM_OC1PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
            } else if (chList[i] == 2) {
                TIM_OC2Init(timTimePort[timNum], &TIM_OCInitStructure);
                TIM_OC2PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
            } else if (chList[i] == 3) {
                TIM_OC3Init(timTimePort[timNum], &TIM_OCInitStructure);
                TIM_OC3PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
            } else if (chList[i] == 4) {
                TIM_OC4Init(timTimePort[timNum], &TIM_OCInitStructure);
                TIM_OC4PreloadConfig(timTimePort[timNum], TIM_OCPreload_Enable);
            }
        }

        TIM_ARRPreloadConfig(timTimePort[timNum], ENABLE);
        TIM_CtrlPWMOutputs(timTimePort[timNum], ENABLE);
    }

    /* 编码器模式 */
    if (mode == encoder) {
        TIM_EncoderInterfaceConfig(timTimePort[timNum],
                                   TIM_EncoderMode_TI12,
                                   TIM_ICPolarity_Rising,
                                   TIM_ICPolarity_Rising);

        TIM_ICInitTypeDef TIM_ICInitStructure = {0};
        TIM_ICInitStructure.TIM_ICPolarity    = TIM_ICPolarity_Rising;
        TIM_ICInitStructure.TIM_ICSelection   = TIM_ICSelection_DirectTI;
        TIM_ICInitStructure.TIM_ICPrescaler   = TIM_ICPSC_DIV1;
        TIM_ICInitStructure.TIM_ICFilter      = 0x0A;

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
                TIM_ICInit(timTimePort[timNum], &TIM_ICInitStructure);
            } else if (chList[i] == 2) {
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
                TIM_ICInit(timTimePort[timNum], &TIM_ICInitStructure);
            } else if (chList[i] == 3) {
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
                TIM_ICInit(timTimePort[timNum], &TIM_ICInitStructure);
            } else if (chList[i] == 4) {
                TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
                TIM_ICInit(timTimePort[timNum], &TIM_ICInitStructure);
            }
        }

        TIM_SetCounter(timTimePort[timNum], 0);
    }

    /* 启动定时器 */
    TIM_Cmd(timTimePort[timNum], ENABLE);
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief      设置 PWM 占空比
 * \param[in]  timNum 定时器编号 1..5, 8..14
 * \param[in]  chNum 通道编号 1..4
 * \param[in]  duty 占空比 范围：0..ARR
 * \note        - chNum 仅支持单通道设置
 * \warning     - 确保私有变量配置正确！
 *              - timNum 必须在 timMapping 映射表中有对应配置!
 */
void timer_pwm_set(u8 timNum, u8 chNum, u16 duty)
{
    /* 定时器 检测与重映射 */
    if (timNum < 1 || timNum > 14) return;
    timNum = timMapping[timNum];

    if (chNum == 1)
        timTimePort[timNum]->CCR1 = duty;
    else if (chNum == 2)
        timTimePort[timNum]->CCR2 = duty;
    else if (chNum == 3)
        timTimePort[timNum]->CCR3 = duty;
    else if (chNum == 4)
        timTimePort[timNum]->CCR4 = duty;
}

/******************************************************************
 * \brief  读取编码器计数值
 * \param[in]  timNum 定时器编号 1..5, 8..14
 * \param[in]  sign 计数值符号选择
 *   \arg       - normal: 正常计数值
 *   \arg       - inverse: 取反计数值
 * \retval 编码器计数值
 * \note        - 读取后会清零计数值
 * \warning     - 确保私有变量配置正确！
 *              - timNum 必须在 timMapping 映射表中有对应配置!
 */
s16 timer_encoder_read(u8 timNum, sign_t sign)
{
    /* 定时器 检测与重映射 */
    if (timNum < 1 || timNum > 14) return 0;
    timNum = timMapping[timNum];

    // 将 uint32_t 强制转换为 int16_t 来正确处理负数
    s16 cnt = (s16)TIM_GetCounter(timTimePort[timNum]); // 读取计数值
    TIM_SetCounter(timTimePort[timNum], 0);             // 清零

    return (sign) ? -cnt : cnt; // 右轮取反
}

/* ******************** 功能函数 */
