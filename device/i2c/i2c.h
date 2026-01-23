/**
 * \page  软件模拟 I2C 驱动代码说明
 *   \li  本文件为软件模拟的 I2C 通信协议的通用驱动代码
 *        参考教程：https://blog.csdn.net/xiaobaivera/article/details/140551276（江协版）
 *
 *   \li  为了增加代码的复用性
 *        本文件不作初始化 GPIO 的工作
 *        同时使用枚举类型 i2cObj_t 来区分不同的 I2C 设备
 */
#ifndef __I2C_H_
#define __I2C_H_

#include "main.h"

/* Global Types ------------------------------------------------------------ */

typedef enum {
    i2cObj_MPU6050 = 1,
    i2cObj_OLED    = 2,
} i2cObj_t;

/* Global Variables -------------------------------------------------------- */

extern volatile u8 i2cIdx;

/* Global Functions -------------------------------------------------------- */

// 通信工具

void i2c_w_SCL(u8 bit);
void i2c_w_SDA(u8 bit);

u8 i2c_r_SCL(void);
u8 i2c_r_SDA(void);

// 通信协议

void i2c_start(void);
void i2c_stop(void);
void i2c_pass(void);

void i2c_send_byte(u8 byte);
u8 i2c_recv_byte(void);

void i2c_send_ack(u8 ack);
u8 i2c_recv_ack(void);

// 初始化函数

void i2c_gpio_init(u8 idx);

#endif
