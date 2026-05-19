#ifndef __TIMER_H
#define __TIMER_H

#include "heads.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// TIM 配置索引
typedef enum {
    tim1 = 0,
    tim2,
    tim3,
    tim4,
    tim5,
    tim6,
    tim7,
    tim8,
    tim9,
    tim10,
    tim11,
    tim12,
    tim13,
    tim14
} tim_e;

// TIM 配置结构体
typedef struct {
    u32 rccGpio;
    u16 chNum;             // 支持多通道无序输入，如：通道一和三：13，通道一、四和二：142（编码器模式下仅支持1/2双通道输入）
    GPIO_TypeDef *gpio[4]; // 四个通道的 GPIO 端口
    u16 pin[4];            // 四个通道的 GPIO 引脚
    u8 af[4];              // 四个通道的 GPIO 复用功能编号
    u8 src[4];             // 四个通道的 GPIO 引脚源编号
    u32 rccTim;
    TIM_TypeDef *tim;
    IRQn_Type irqn;
} tim_s;

// TIM 配置模式
typedef enum {
    timMode_timer = 0,
    timMode_pwmIn,
    timMode_pwmOut,
    timMode_encoder,
} timMode_e;

// PWM 输入测量结果
typedef struct {
    u16 period; // 周期计数值（CCR1）
    u16 high;   // 高电平计数值（CCR2）
    u32 freq;   // 频率（Hz）
    u16 duty;   // 占空比（0..10000 => 0.00%..100.00%）
} timPwmInData_s;

/* Global Variables -------------------------------------------------------- */

extern __IO timPwmInData_s timPwmInData;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

bool timer_init(timMode_e mode, tim_e idx, u16 arr, u16 psc, u8 priority);

// 设置函数

bool timer_pwmIn_readData(tim_e idx, __IO timPwmInData_s *data);
bool timer_pwmOut_setDuty(tim_e idx, u8 chNum, u16 duty);
s16 timer_encoder_readCnt(tim_e idx, sign_e sign);

#endif
