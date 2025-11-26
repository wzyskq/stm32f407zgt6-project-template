#include "gpio.h"

// 初始化PA2
void gpio_init_pa2(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // 使能GPIOA时钟

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         // 选择PA2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;     // 设置为输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置速度为50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 设置为推挽输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 不使用上拉或下拉电阻
    GPIO_Init(GPIOA, &GPIO_InitStructure);            // 初始化GPIOA2
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);                // 设置PA2为低电平
}
