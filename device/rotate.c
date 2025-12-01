#include "rotate.h"

/**
 * \brief 初始化旋转电机控制 GPIO
 */
void rotate_init(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOG, GPIO_Pin_10);
    GPIO_ResetBits(GPIOG, GPIO_Pin_11);
    GPIO_ResetBits(GPIOG, GPIO_Pin_14);
    GPIO_ResetBits(GPIOG, GPIO_Pin_15);
}

// 整体控制

/**
 * \brief 整体停止
 */

void rotate_stop(void)
{
    rotate_stop_lt();
    rotate_stop_rt();
}

/**
 * \brief 整体向前
 */
void rotate_forward(void)
{
    rotate_forward_lt();
    rotate_forward_rt();
}

/**
 * \brief 整体向后
 */
void rotate_backward(void)
{
    rotate_backward_lt();
    rotate_backward_rt();
}

// 右轮控制

/**
 * \brief 右轮停止
 */
void rotate_stop_rt(void)
{
    GPIO_SetBits(GPIOG, GPIO_Pin_10);
    GPIO_SetBits(GPIOG, GPIO_Pin_11);
}
/**
 * \brief 右轮向前
 */
void rotate_forward_rt(void)
{
    GPIO_SetBits(GPIOG, GPIO_Pin_10);
    GPIO_ResetBits(GPIOG, GPIO_Pin_11);
}

/**
 * \brief 右轮向后
 */
void rotate_backward_rt(void)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_10);
    GPIO_SetBits(GPIOG, GPIO_Pin_11);
}

// 左轮控制

/**
 * \brief 左轮停止
 */
void rotate_stop_lt(void)
{
    GPIO_SetBits(GPIOG, GPIO_Pin_14);
    GPIO_SetBits(GPIOG, GPIO_Pin_15);
}

/**
 * \brief 左轮向前
 */
void rotate_forward_lt(void)
{
    GPIO_ResetBits(GPIOG, GPIO_Pin_14);
    GPIO_SetBits(GPIOG, GPIO_Pin_15);
}

/**
 * \brief 左轮向后
 */
void rotate_backward_lt(void)
{
    GPIO_SetBits(GPIOG, GPIO_Pin_14);
    GPIO_ResetBits(GPIOG, GPIO_Pin_15);
}
