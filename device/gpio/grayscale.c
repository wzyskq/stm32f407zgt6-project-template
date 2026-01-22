#include "grayscale.h"

/* Private Macros ---------------------------------------------------------- */

#define GRAY_NUM 8 // 灰度传感器数量

/* Private Variables ------------------------------------------------------- */

static const u32 grayRccGpio[] = {
    0,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
    RCC_AHB1Periph_GPIOA,
};

static GPIO_TypeDef *grayGpioPort[] = {
    0,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
    GPIOA,
};

static const u16 grayGpioPin[] = {
    0,
    GPIO_Pin_12,
    GPIO_Pin_11,
    GPIO_Pin_8,
    GPIO_Pin_7,
    GPIO_Pin_6,
    GPIO_Pin_5,
    GPIO_Pin_4,
    GPIO_Pin_2,
};

// 灰度传感器权重数组
static const s8 grayWeight[GRAY_NUM + 1] = {
    0,
    -35, -25, -15, -5, 5, 15, 25, 35};

/* Global Variables -------------------------------------------------------- */

u8 graySrc[GRAY_NUM + 1] = {0}; // 灰度传感器状态数组

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化指定灰度传感器
 * \param[in]  grayNum 灰度传感器编号，x=1..8
 * \note       强烈建议使用 grays_init() 初始化所有灰度传感器
 */
void gray_init(u8 grayNum)
{
    if (grayNum < 1 || grayNum > GRAY_NUM) return;

    RCC_AHB1PeriphClockCmd(grayRccGpio[grayNum], ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = grayGpioPin[grayNum]; // 指定按键
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     // 50MHz
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;         // 普通输入模式
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         // 上拉
    GPIO_Init(grayGpioPort[grayNum], &GPIO_InitStructure);
}

/******************************************************************
 * \brief  初始化所有灰度传感器
 * \note   请先确保私有宏 GRAY_NUM 已正确设置
 */
void grays_init(void)
{
    for (u8 i = 1; i <= GRAY_NUM; i++)
        gray_init(i);
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief  灰度传感器检测
 * \return 返回检测到的灰度传感器编号，0表示未检测到
 */
void gray_scan(void)
{
    graySrc[0] = 0; // 清除长度记录
    for (u8 i = 1; i <= GRAY_NUM; i++)
        graySrc[i] = GPIO_ReadInputDataBit(grayGpioPort[i], grayGpioPin[i]);
}

/* ******************** 功能函数 */

/*





*/

/* 算法函数 ******************** */

/******************************************************************
 * \brief       灰度传感器滤波算法
 * \param[in]   src 灰度传感器状态数组指针
 * \param[out]  res 灰度传感器滤波结果数组指针
 * \return      返回滤波后的起始值下标
 * \note        原理：求最长连续相同状态长度
 */
u8 gray_filter_core(u8 *src, u8 *res)
{
    u8 ln0 = 0; // 当前段长度
    u8 sn0 = 0; // 当前段起始下标
    u8 lm  = 0; // 最大段长度
    u8 sm  = 0; // 最大段起始下标

    // 计算长度
    for (u8 i = 1; i <= GRAY_NUM; i++) {
        if (src[i]) {
            if (!src[i - 1])
                ln0 = 1, sn0 = i;
            else
                ln0++;
            if (i == GRAY_NUM)
                if (ln0 > lm) lm = ln0, sm = sn0;
        } else {
            if (src[i - 1])
                if (ln0 > lm) lm = ln0, sm = sn0;
        }
    }

    // 导出数组
    for (int i = 0; i <= GRAY_NUM; i++)
        if (sm <= i && i < sm + lm)
            res[i] = 1;
        else
            res[i] = 0;

    res[0] = lm;
    return sm;
}

/******************************************************************
 * \brief       灰度传感器量化算法
 * \param[in]   src 灰度传感器状态数组指针
 * \param[out]  res 灰度传感器滤波结果数组指针
 * \return      返回量化结果
 * \note        原理：加权平均法
 */
s8 gray_quant_core(u8 *src, u8 *res)
{
    static s8 v0 = 0; // 上一次量化结果

    s8 v = 0; // 量化结果
    s8 s = 0; // 权重和

    u8 sm = gray_filter_core(src, res);
    for (u8 i = sm; i < sm + res[0]; i++)
        s += grayWeight[i];

    v = (res[0] == 0) ? 0 : (s / res[0]);

    // 超限保持算法
    if (v == 0) {
        if (v0 == 35 || v0 == -35)
            v = v0; // 保持上一次的极值
    }
    v0 = v;

    return v;
}

/* ******************** 算法函数 */
