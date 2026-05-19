#ifndef __KEY_H_
#define __KEY_H_

#include "main.h"

/* Global Macros ----------------------------------------------------------- */

/* Private Types ----------------------------------------------------------- */

// KEY 配置索引
typedef enum {
    key0 = 0, // 板载按键
    key1,
    key2,
    key3,
    key4,
    key5,
    key6,
    keyNum // KEY 末尾索引，仅用于统计 KEY 数量
} key_e;

// KEY 配置结构体
typedef struct
{
    u32 rccGpio;
    GPIO_TypeDef *gpio;
    u16 pin;
    level_e keyOnLevel; // KEY 按下电平
    u16 overtime;       // 超时时间（单位：定时器中断周期）
} key_s;

// KEY 状态结构体
typedef struct
{
    u8 idx;       // 按键索引
    u8 cnt;       // 按键次数
    u16 tim;      // 按键时间（单位：定时器中断周期）
    u16 overtime; // 超时时间（单位：定时器中断周期）
} keySts_s;

/* Global Variables -------------------------------------------------------- */

extern keySts_s keySts;
extern u8 taskNum;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void key_init(key_e idx);
void keys_init(void);

// 功能函数

bool key_read(key_e idx, bool block);
key_e key_scan(bool block);
void key_judge(void);

#endif
