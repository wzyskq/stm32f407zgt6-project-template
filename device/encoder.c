#include "encoder.h"

// 编码器配置数据
static const u32 rccTim[3] = {0, RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM4};
static const u32 rccGpio[3] = {0, RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB};

static const GPIO_TypeDef *encGpio[3] = {0, GPIOA, GPIOB};
static const TIM_TypeDef *encTim[3] = {0, TIM5, TIM4};

static const u8 encGpioAF[3] = {0, GPIO_AF_TIM5, GPIO_AF_TIM4};
static const u16 encGpioPin1[3] = {0, GPIO_Pin_0, GPIO_Pin_6}; // 左轮PA0, 右轮PB6
static const u16 encGpioPin2[3] = {0, GPIO_Pin_1, GPIO_Pin_7}; // 左轮PA1, 右轮PB7
static const u8 encGpioPinSrc1[3] = {0, GPIO_PinSource0, GPIO_PinSource6};
static const u8 encGpioPinSrc2[3] = {0, GPIO_PinSource1, GPIO_PinSource7};

/**
 * \brief  编码器初始化
 * \param  encNum 编码器号
 *   \arg  1: 左轮 (TIM5)
 *   \arg  2: 右轮 (TIM4)
 *
 * \note   左轮: PA0/PA1 (TIM5_CH1/TIM5_CH2)
 *         右轮: PB6/PB7 (TIM4_CH1/TIM4_CH2)
 */
void encoder_init(u8 encNum)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(rccTim[encNum], ENABLE);  // 开启TIMx的时钟
    RCC_AHB1PeriphClockCmd(rccGpio[encNum], ENABLE); // 开启GPIOx的时钟

    /*GPIO初始化 - 修改为输入模式*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = encGpioPin1[encNum] | encGpioPin2[encNum];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       // 复用功能模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 提高GPIO速度
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // 改为上拉，提供稳定的信号参考
    GPIO_Init((GPIO_TypeDef *)encGpio[encNum], &GPIO_InitStructure);

    /*连接引脚到TIMx*/
    GPIO_PinAFConfig((GPIO_TypeDef *)encGpio[encNum], encGpioPinSrc1[encNum], encGpioAF[encNum]);
    GPIO_PinAFConfig((GPIO_TypeDef *)encGpio[encNum], encGpioPinSrc2[encNum], encGpioAF[encNum]);

    /*TIM时基初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF; // 使用最大值65535
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit((TIM_TypeDef *)encTim[encNum], &TIM_TimeBaseStructure);

    /*编码器接口配置 - 简化配置*/
    TIM_EncoderInterfaceConfig((TIM_TypeDef *)encTim[encNum],
                               TIM_EncoderMode_TI12,   // 使用TI1和TI2
                               TIM_ICPolarity_Rising,  // TI1上升沿
                               TIM_ICPolarity_Rising); // TI2上升沿

    /*设置滤波器 - 降低滤波值*/
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x0A; // 滤波值
    TIM_ICInit((TIM_TypeDef *)encTim[encNum], &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit((TIM_TypeDef *)encTim[encNum], &TIM_ICInitStructure);

    /*清零并启动定时器*/
    TIM_SetCounter((TIM_TypeDef *)encTim[encNum], 0);
    TIM_Cmd((TIM_TypeDef *)encTim[encNum], ENABLE);
}

/**
 * \brief  读取编码器计数值绝对值
 * \param  encNum 编码器号
 *   \arg  1: 左轮 (TIM5)
 *   \arg  2: 右轮 (TIM4)
 * \retval 编码器计数值
 */
s16 encoder_read(u8 encNum)
{
    if (encNum == 1 || encNum == 2)
    {
        // 将uint32_t强制转换为int16_t来正确处理负数
        s16 cnt = (s16)TIM_GetCounter((TIM_TypeDef *)encTim[encNum]);
        TIM_SetCounter((TIM_TypeDef *)encTim[encNum], 0); // 读取后清零

        // // 调试输出原始计数值
        // if (cnt)
        // if (dataEngine == rawDate)
        // serial_printf(USART1, "ENC%d raw: %d\n", encNum, cnt);

        return (encNum == 2) ? -cnt : cnt; // 右轮取反
    }
    return 0;
}
