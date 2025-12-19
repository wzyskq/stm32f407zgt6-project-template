#include "wheel.h"

/* Global Macros ----------------------------------------------------------- */

#define WHL_NUM 2 // 轮子数量

/* Private Variables ------------------------------------------------------- */

// 私有常量数组（可自定义）

/******************************************************************
 * \brief    轮子对应的定时器及通道映射表
 * \note      - 每个轮子对应一个定时器的单通道 PWM 输出！
 *            - 当前映射表 timer.c 中的 timMapping 映射表无关！
 * \example  {1, 3}, {1, 4}, etc.
 *           定时器1的通道3和通道4分别控制两个轮子
 */
static const u8 timMapping[][2] = {
    {0, 0},
    {1, 3}, // 左轮 TIM1通道3
    {1, 4}, // 右轮 TIM1通道4
};

/******************************************************************
 * \brief    电机 RCC 外设时钟使能映射表
 * \note      - 仅需列出常用定时器的 GPIO 外设时钟使能宏定义
 *            - 默认控制一个轮子的两个引脚在同一个 GPIO 端口
 * \extends   - AHB1 总线: GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
 * \example  RCC_AHB1Periph_GPIOA, RCC_AHB1Periph_GPIOB, etc.
 */
static const u32 whlRccGpio[] = {
    0,
    RCC_AHB1Periph_GPIOG,
    RCC_AHB1Periph_GPIOG,
};

/******************************************************************
 * \brief    电机 GPIO 端口映射表
 * \note      - 默认控制一个轮子的两个引脚在同一个 GPIO 端口
 * \extends   - GPIO 端口类型: GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG
 * \example  GPIOA, GPIOB, etc.
 */
static GPIO_TypeDef *whlGpioPort[] = {
    0,
    GPIOG,
    GPIOG,
};

/******************************************************************
 * \brief    电机 GPIO 引脚映射表
 * \note      - 每个轮子使用两个引脚控制前进与后退
 * \extends   - GPIO_Pin_x, x=0..15
 * \example  {GPIO_Pin_8, GPIO_Pin_9}, etc.
 */
static const u16 whlGpioPin[][2] = {
    {0, 0},
    {GPIO_Pin_15, GPIO_Pin_14}, // 左轮
    {GPIO_Pin_10, GPIO_Pin_11}, // 右轮
};

/*





*/

/* Global Variables -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      受控电机 GPIO 初始化
 * \param[in]  whlNum 轮子编号
 * \note        - 建议使用 wheels_init() 初始化所有轮子
 * \warning     - 确保私有变量配置正确！
 */
void wheel_init(u8 whlNum)
{
    /* 轮子检测 */
    if (whlNum < 1 || whlNum > WHL_NUM) return;

    RCC_AHB1PeriphClockCmd(whlRccGpio[whlNum], ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    for (u8 i = 0; i < 2; i++) {
        GPIO_InitStructure.GPIO_Pin = whlGpioPin[whlNum][i];
        GPIO_Init(whlGpioPort[whlNum], &GPIO_InitStructure);
        GPIO_ResetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][i]);
    }
}

/******************************************************************
 * \brief      初始化所有轮子
 * \note       请先确保私有宏 WHL_NUM 已正确设置
 * \warning     - 确保私有变量配置正确！
 */
void wheels_init(void)
{
    for (u8 i = 1; i <= WHL_NUM; i++)
        wheel_init(i);
}

/* ******************** 初始化函数 */

/*





*/

/* 工具函数 ******************** */

/******************************************************************
 * \brief      轮子停止
 * \param[in]  whlNum 轮子编号
 * \warning     - 确保私有变量配置正确！
 */
void wheel_stop(u8 whlNum)
{
    if (whlNum < 1 || whlNum > WHL_NUM) return;

    GPIO_SetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][0]);
    GPIO_SetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][1]);
}

/*******************************************************************
 * \brief      轮子向前
 * \param[in]  whlNum 轮子编号
 * \warning     - 确保私有变量配置正确！
 */
void wheel_forward(u8 whlNum)
{
    if (whlNum < 1 || whlNum > WHL_NUM) return;

    GPIO_SetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][0]);
    GPIO_ResetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][1]);
}

/******************************************************************
 * \brief      轮子向后
 * \param[in]  whlNum 轮子编号
 * \warning     - 确保私有变量配置正确！
 */
void wheel_backward(u8 whlNum)
{
    if (whlNum < 1 || whlNum > WHL_NUM) return;

    GPIO_ResetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][0]);
    GPIO_SetBits(whlGpioPort[whlNum], whlGpioPin[whlNum][1]);
}

/* ******************** 工具函数 */

/*





*/

/* 功能函数 ******************** */

/**
 * \brief      设置轮子速度
 * \pre        配置轮子对应的定时器及通道映射表 timMapping
 * \param[in]  whlNum 轮子编号
 * \param[in]  pwm 速度(脉冲)值，范围：[-ARR, ARR]
 * \warning     - 确保私有变量配置正确！
 */
void wheel_pwm_set(u8 whlNum, s16 pwm)
{
    if (whlNum < 1 || whlNum > WHL_NUM) return;

    if (pwm >= 70)
        wheel_forward(whlNum);
    else if (pwm <= -70)
        wheel_backward(whlNum);
    else
        wheel_stop(whlNum);

    timer_pwm_set(timMapping[whlNum][0], timMapping[whlNum][1], abs(pwm));
}

/* ******************** 功能函数 */
