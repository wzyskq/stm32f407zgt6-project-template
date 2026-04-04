#ifndef __TYPES_H
#define __TYPES_H

/* Global Types ------------------------------------------------------------ */

// 电平类型
typedef enum {
    low  = 0,
    high = !low,
} level_e;

// 方向类型
typedef enum {
    normal = 0,
    inverse = !normal,
} sign_e;

/* timer.h ******************** */

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

/* serial.h ******************** */

// USART 配置索引
typedef enum {
    usart1 = 0,
    usart2,
    usart3,
    uart4,
} srl_e;

#endif
