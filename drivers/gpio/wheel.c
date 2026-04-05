#include "wheel.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const wheel_s whlList[] = {
    [wheelLt] = {
        .rccGpio = RCC_AHB1Periph_GPIOD,
        .gpio    = GPIOD,
        .pin     = {GPIO_Pin_3, GPIO_Pin_4},
        .tim     = tim1,
        .ch      = 3,
    },
    [wheelRt] = {
        .rccGpio = RCC_AHB1Periph_GPIOG,
        .gpio    = GPIOG,
        .pin     = {GPIO_Pin_11, GPIO_Pin_10},
        .tim     = tim1,
        .ch      = 4,
    },
};

/* Global Variables -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      受控电机 GPIO 初始化
 * \param[in]  idx 轮子编号
 * \note        - 建议使用 wheels_init() 初始化所有轮子
 *              - 请先确保 keyNum 置于配置索引末尾
 */
void wheel_init(wheel_e idx)
{
    /* 轮子检测 */
    if (idx >= wheelNum) return;

    RCC_AHB1PeriphClockCmd(whlList[idx].rccGpio, ENABLE);

    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Mode  = GPIO_Mode_OUT;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    for (u8 i = 0; i < 2; i++) {
        gpio.GPIO_Pin = whlList[idx].pin[i];
        GPIO_Init(whlList[idx].gpio, &gpio);
        GPIO_ResetBits(whlList[idx].gpio, whlList[idx].pin[i]);
    }
}

/******************************************************************
 * \brief  初始化所有轮子
 * \note   请先确保 keyNum 置于配置索引末尾
 */
void wheels_init(void)
{
    for (u8 i = 0; i < wheelNum; i++)
        wheel_init((wheel_e)i);
}

/* ******************** 初始化函数 */

/*





*/

/* 工具函数 ******************** */

/******************************************************************
 * \brief      轮子停止
 * \param[in]  idx 轮子编号
 */
void wheel_stop(wheel_e idx)
{
    if (idx >= wheelNum) return;

    GPIO_SetBits(whlList[idx].gpio, whlList[idx].pin[0]);
    GPIO_SetBits(whlList[idx].gpio, whlList[idx].pin[1]);
}

/*******************************************************************
 * \brief      轮子向前
 * \param[in]  idx 轮子编号
 */
void wheel_forward(wheel_e idx)
{
    if (idx >= wheelNum) return;

    GPIO_SetBits(whlList[idx].gpio, whlList[idx].pin[0]);
    GPIO_ResetBits(whlList[idx].gpio, whlList[idx].pin[1]);
}

/******************************************************************
 * \brief      轮子向后
 * \param[in]  idx 轮子编号
 */
void wheel_backward(wheel_e idx)
{
    if (idx >= wheelNum) return;

    GPIO_ResetBits(whlList[idx].gpio, whlList[idx].pin[0]);
    GPIO_SetBits(whlList[idx].gpio, whlList[idx].pin[1]);
}

/* ******************** 工具函数 */

/*





*/

/* 功能函数 ******************** */

/**
 * \brief      设置轮子速度
 * \pre        配置轮子对应的定时器及通道映射表 timMapping
 * \param[in]  idx 轮子编号
 * \param[in]  pwm 速度(脉冲)值，范围：-10000..10000
 * \note       idx 为 wheelNum 时，设置所有轮子速度
 */
void wheel_pwm_set(wheel_e idx, s16 pwm)
{
    if (idx >= wheelNum) return;

    u8 i = (idx == wheelNum) ? 0 : idx;
    u8 m = (idx == wheelNum) ? wheelNum - 1 : idx;  

    for (; i <= m; i++) {
        if (pwm > 0)
            wheel_forward((wheel_e)i);
        else if (pwm < 0)
            wheel_backward((wheel_e)i);
        else
            wheel_stop((wheel_e)i);

        timer_pwmOut_setDuty(whlList[i].tim, whlList[i].ch, (u16)ABS(pwm));
    }
    return;
}

/* ******************** 功能函数 */
