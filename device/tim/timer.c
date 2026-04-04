#include "timer.h"

/* Private Macros ---------------------------------------------------------- */

#define NONE_IRQn ((IRQn_Type)-128)

/* Private variables ------------------------------------------------------- */

/******************************************************************
 * \extends   - APB1 总线: TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14
 *            - APB2 总线: TIM1, TIM8, TIM9, TIM10, TIM11
 *
 * \extends   - AHB1 总线: GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
 *
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
 */

static const tim_s timList[] = {
    [tim1] = { // 转速控制
        .rccGpio = RCC_AHB1Periph_GPIOE,
        .chNum   = 12,
        .gpio    = {0, 0, GPIOE, GPIOE},
        .pin     = {0, 0, GPIO_Pin_13, GPIO_Pin_14},
        .af      = {0, 0, GPIO_AF_TIM1, GPIO_AF_TIM1},
        .src     = {0, 0, GPIO_PinSource13, GPIO_PinSource14},
        .rccTim  = RCC_APB2Periph_TIM1,
        .tim     = TIM1,
        .irqn    = NONE_IRQn,
    },
    [tim3] = { // 左轮编码器
        .rccGpio = RCC_AHB1Periph_GPIOC,
        .chNum   = 12,
        .gpio    = {GPIOC, GPIOC, 0, 0},
        .pin     = {GPIO_Pin_6, GPIO_Pin_7, 0, 0},
        .af      = {GPIO_AF_TIM3, GPIO_AF_TIM3, 0, 0},
        .src     = {GPIO_PinSource6, GPIO_PinSource7, 0, 0},
        .rccTim  = RCC_APB1Periph_TIM3,
        .tim     = TIM3,
        .irqn    = NONE_IRQn,
    },
    [tim4] = { // 右轮编码器
        .rccGpio = RCC_AHB1Periph_GPIOB,
        .chNum   = 12,
        .gpio    = {GPIOB, GPIOB, 0, 0},
        .pin     = {GPIO_Pin_6, GPIO_Pin_7, 0, 0},
        .af      = {GPIO_AF_TIM4, GPIO_AF_TIM4, 0, 0},
        .src     = {GPIO_PinSource6, GPIO_PinSource7, 0, 0},
        .rccTim  = RCC_APB1Periph_TIM4,
        .tim     = TIM4,
        .irqn    = NONE_IRQn,
    },
    [tim7] = { // 系统时基
        .rccGpio = 0,
        .chNum   = 0,
        .gpio    = {0, 0, 0, 0},
        .pin     = {0, 0, 0, 0},
        .af      = {0, 0, 0, 0},
        .src     = {0, 0, 0, 0},
        .rccTim  = RCC_APB1Periph_TIM7,
        .tim     = TIM7,
        .irqn    = TIM7_IRQn,
    },
};

/* Global Variables -------------------------------------------------------- */

__IO timPwmInData_s timPwmInData; // PWM 输入测量结果

/* Private Functions ------------------------------------------------------- */

/******************************************************************
 * \brief      获取定时器时钟频率
 * \param[in]  idx 定时器编号
 * \retval     定时器时钟频率（Hz）
 */
static u32 timer_getClkHz(tim_e idx)
{
    RCC_ClocksTypeDef clocks = {0};
    RCC_GetClocksFreq(&clocks);

    if (idx == tim1 || (idx >= tim8 && idx <= tim11)) {
        u32 clk = clocks.PCLK2_Frequency;
        if ((RCC->CFGR & RCC_CFGR_PPRE2) != 0)
            clk <<= 1;
        return clk;
    }

    u32 clk = clocks.PCLK1_Frequency;
    if ((RCC->CFGR & RCC_CFGR_PPRE1) != 0)
        clk <<= 1;
    return clk;
}

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      定时器初始化
 * \param[in]  mode 初始化模式
 *   \arg        timMode_timer，定时器模式
 *   \arg        timMode_pwmIn，PWM 输入模式
 *   \arg        timMode_pwmOut，PWM 输出模式
 *   \arg        timMode_encoder，编码器模式
 * \param[in]  idx 定时器编号 1..14
 * \param[in]  arr 自动重装载值 + 1
 * \param[in]  psc 预分频值 + 1
 * \param[in]  priority 优先级，格式：抢占优先级+响应优先级。例：01 表示抢占优先级0，响应优先级1
 *
 * \retval     bool 初始化成功与否
 */
bool timer_init(timMode_e mode, tim_e idx, u16 arr, u16 psc, u8 priority)
{
    /* 基本参数检查 */
    if (idx > tim14)
        return false;
    if (timList[idx].tim == 0 || timList[idx].rccTim == 0)
        return false;

    /* 功能定时器检查 */
    if ((mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder) && (idx == tim6 || idx == tim7))
        return false;

    /* 定时模式参数检查 */
    if ((mode == timMode_timer || mode == timMode_pwmOut) && (arr == 0 || psc == 0))
        return false;

    /* 解析通道号 */
    u8 chList[5] = {0};
    u16 chNum    = timList[idx].chNum;
    while (chNum) {
        u8 digit = chNum % 10;
        if (digit >= 1 && digit <= 4)
            chList[++chList[0]] = digit;
        chNum /= 10;
    }

    // pwmIn/pwmOut/encoder 模式至少需要一个通道，且 PWM 输入模式必须使用 CH1/CH2
    if ((mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder) && chList[0] == 0)
        return false;

    // PWM 输入模式依赖 CH1/CH2
    if (mode == timMode_pwmIn && !(timList[idx].chNum == 12 || timList[idx].chNum == 21))
        return false;

    /* 时钟使能 */
    if (idx == tim1 || (idx >= tim8 && idx <= tim11))
        RCC_APB2PeriphClockCmd(timList[idx].rccTim, ENABLE); // 高级/高速总线定时器
    else
        RCC_APB1PeriphClockCmd(timList[idx].rccTim, ENABLE); // 通用、基本定时器
    if (mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder)
        RCC_AHB1PeriphClockCmd(timList[idx].rccGpio, ENABLE); // GPIO 时钟

    TIM_DeInit(timList[idx].tim); // 复位定时器寄存器到默认值，避免之前配置干扰

    /* GPIO 配置 */
    if (mode == timMode_pwmIn || mode == timMode_pwmOut || mode == timMode_encoder) {
        GPIO_InitTypeDef gpio = {0};
        gpio.GPIO_Mode        = GPIO_Mode_AF;
        gpio.GPIO_Speed       = GPIO_Speed_100MHz;
        gpio.GPIO_OType       = GPIO_OType_PP;
        gpio.GPIO_PuPd        = GPIO_PuPd_UP;

        for (u8 i = 1; i <= chList[0]; i++) {
            u8 ch = (u8)(chList[i] - 1);
            GPIO_PinAFConfig(timList[idx].gpio[ch], timList[idx].src[ch], timList[idx].af[ch]);

            gpio.GPIO_Pin = timList[idx].pin[ch];
            GPIO_Init(timList[idx].gpio[ch], &gpio);
        }
    }

    /* 定时器配置 */
    TIM_TimeBaseInitTypeDef tb = {0};
    tb.TIM_Period              = (mode == timMode_encoder) ? 0xFFFF : (u16)(arr - 1); // encoder 模式固定全计数
    tb.TIM_Prescaler           = (mode == timMode_encoder) ? 0 : (u16)(psc - 1);      // encoder 模式不分频
    tb.TIM_ClockDivision       = TIM_CKD_DIV1;
    tb.TIM_CounterMode         = TIM_CounterMode_Up;
    tb.TIM_RepetitionCounter   = 0;
    TIM_TimeBaseInit(timList[idx].tim, &tb);

    /* TIM 定时模式中断配置 */
    if (mode == timMode_timer) {
        TIM_ClearITPendingBit(timList[idx].tim, TIM_IT_Update); // 清除首次更新标志

        NVIC_InitTypeDef nvic                  = {0};
        nvic.NVIC_IRQChannel                   = timList[idx].irqn;
        nvic.NVIC_IRQChannelPreemptionPriority = PreemptingPriority(priority); // 抢占优先级
        nvic.NVIC_IRQChannelSubPriority        = SubPriority(priority);        // 响应优先级
        nvic.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&nvic);

        TIM_ITConfig(timList[idx].tim, TIM_IT_Update, ENABLE); // 允许更新中断
    }

    // PWM 输入模式配置
    else if (mode == timMode_pwmIn) {
        TIM_ICInitTypeDef ic = {0};
        ic.TIM_Channel       = TIM_Channel_1; // 以 CH1 为基准，自动配对 CH2
        ic.TIM_ICPolarity    = TIM_ICPolarity_Rising;
        ic.TIM_ICSelection   = TIM_ICSelection_DirectTI;
        ic.TIM_ICPrescaler   = TIM_ICPSC_DIV1;
        ic.TIM_ICFilter      = 0x0A;

        TIM_PWMIConfig(timList[idx].tim, &ic);
        TIM_SelectInputTrigger(timList[idx].tim, TIM_TS_TI1FP1);
        TIM_SelectSlaveMode(timList[idx].tim, TIM_SlaveMode_Reset);
        TIM_SelectMasterSlaveMode(timList[idx].tim, TIM_MasterSlaveMode_Enable);

        TIM_ClearFlag(timList[idx].tim, TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_Update);
    }

    // PWM 输出模式配置
    else if (mode == timMode_pwmOut) {
        TIM_OCInitTypeDef oc = {0};
        TIM_OCStructInit(&oc);
        oc.TIM_OCMode      = TIM_OCMode_PWM1;        // PWM 模式 1
        oc.TIM_OutputState = TIM_OutputState_Enable; // 使能输出
        oc.TIM_OCPolarity  = TIM_OCPolarity_High;    // 高电平有效
        oc.TIM_Pulse       = 0;                      // 初始占空比 0

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                TIM_OC1Init(timList[idx].tim, &oc);
                TIM_OC1PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 2) {
                TIM_OC2Init(timList[idx].tim, &oc);
                TIM_OC2PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 3) {
                TIM_OC3Init(timList[idx].tim, &oc);
                TIM_OC3PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            } else if (chList[i] == 4) {
                TIM_OC4Init(timList[idx].tim, &oc);
                TIM_OC4PreloadConfig(timList[idx].tim, TIM_OCPreload_Enable);
            }
        }

        TIM_ARRPreloadConfig(timList[idx].tim, ENABLE); // 使能自动重装载缓冲
        if (idx == tim1 || idx == tim8)
            TIM_CtrlPWMOutputs(timList[idx].tim, ENABLE); // 高级定时器主输出使能
    }

    // 编码器模式配置
    else if (mode == timMode_encoder) {
        TIM_EncoderInterfaceConfig(timList[idx].tim,
                                   TIM_EncoderMode_TI12,   // 默认 1/2 双通道编码器模式
                                   TIM_ICPolarity_Rising,  // CH1 上升沿
                                   TIM_ICPolarity_Rising); // CH2 上升沿

        TIM_ICInitTypeDef ic = {0};
        ic.TIM_ICPolarity    = TIM_ICPolarity_Rising;
        ic.TIM_ICSelection   = TIM_ICSelection_DirectTI;
        ic.TIM_ICPrescaler   = TIM_ICPSC_DIV1;
        ic.TIM_ICFilter      = 0x0A; // 输入滤波

        for (u8 i = 1; i <= chList[0]; i++) {
            if (chList[i] == 1) {
                ic.TIM_Channel = TIM_Channel_1;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 2) {
                ic.TIM_Channel = TIM_Channel_2;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 3) {
                ic.TIM_Channel = TIM_Channel_3;
                TIM_ICInit(timList[idx].tim, &ic);
            } else if (chList[i] == 4) {
                ic.TIM_Channel = TIM_Channel_4;
                TIM_ICInit(timList[idx].tim, &ic);
            }
        }

        TIM_SetCounter(timList[idx].tim, 0); // 编码器计数清零
    }

    TIM_Cmd(timList[idx].tim, ENABLE); // 启动定时器
    return true;
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief       读取 PWM 输入信息（轮询方式）
 * \param[in]   idx TIM 索引（需配置 CH1/CH2）
 * \param[out]  data PWM 输入测量结果
 * \retval      bool 是否读取成功
 */
bool timer_pwmIn_readData(tim_e idx, __IO timPwmInData_s *data)
{
    if (idx == tim6 || idx == tim7 || idx > tim14)
        return false;
    if (timList[idx].tim == 0)
        return false;
    if (data == NULL)
        return false;

    u16 prd = TIM_GetCapture1(timList[idx].tim);
    u16 hig = TIM_GetCapture2(timList[idx].tim);
    if (prd == 0)
        return false;
    if (hig > prd)
        hig = prd;

    data->period = prd;
    data->high   = hig;
    data->duty   = (u16)roundf(hig * 10000.0f / prd);

    u32 timClkHz = timer_getClkHz(idx);
    u32 pscDiv   = (u32)TIM_GetPrescaler(timList[idx].tim) + 1U;
    data->freq   = (timClkHz == 0 || pscDiv == 0) ? 0 : (timClkHz / pscDiv / prd);

    return true;
}

/******************************************************************
 * \brief      设置 PWM 占空比
 * \param[in]  idx TIM 索引 1..5, 8..14
 * \param[in]  chNum 通道编号 1..4
 * \param[in]  duty 占空比 0..10000 (0.00%..100.00%)
 * \retval     bool 是否设置成功
 */
bool timer_pwmOut_setDuty(tim_e idx, u8 chNum, u16 duty)
{
    if (idx == tim6 || idx == tim7 || idx > tim14)
        return false;
    if (timList[idx].tim == 0)
        return false;
    if (duty > 10000)
        duty = 10000;

    // u32 arr   = (u32)timList[idx].tim->ARR + 1U;
    // u32 pulse = (u32)duty * arr / 10000U;

    // ARR+1 为 10000，则直接映射 duty 到 CCR 寄存器
    u32 pulse = (u32)duty;

    if (chNum == 1)
        timList[idx].tim->CCR1 = pulse;
    else if (chNum == 2)
        timList[idx].tim->CCR2 = pulse;
    else if (chNum == 3)
        timList[idx].tim->CCR3 = pulse;
    else if (chNum == 4)
        timList[idx].tim->CCR4 = pulse;
    else
        return false;

    return true;
}

/******************************************************************
 * \brief  读取编码器计数值
 * \param[in]  idx 定时器编号 1..5, 8..14
 * \param[in]  sign 计数值符号选择
 *   \arg       - normal: 正常计数值
 *   \arg       - inverse: 取反计数值
 * \retval     编码器计数值
 * \note       读取后会清零计数值
 */
s16 timer_encoder_readCnt(tim_e idx, sign_e sign)
{
    if (idx == tim6 || idx == tim7 || idx > tim14)
        return 0;
    if (timList[idx].tim == 0)
        return 0;

    // 将 uint32_t 强制转换为 int16_t 来正确处理负数
    s16 cnt = (s16)TIM_GetCounter(timList[idx].tim); // 读取计数值
    TIM_SetCounter(timList[idx].tim, 0);             // 清零

    return (sign) ? -cnt : cnt;
}

/* ******************** 功能函数 */
