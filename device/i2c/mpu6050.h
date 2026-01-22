/**
 * \page  MPU6050 六轴传感器驱动代码说明
 *   \li  参考链接： https://blog.csdn.net/m0_74082999/article/details/155985159
 */
#ifndef __MPU6050_H_
#define __MPU6050_H_

#include "main.h"
#include "mpu6050_reg.h"

/* Global Types ------------------------------------------------------------ */

// MPU6050 原始数据结构体
typedef struct {
    s16 accel_x;
    s16 accel_y;
    s16 accel_z;
    s16 temp;
    s16 gyro_x;
    s16 gyro_y;
    s16 gyro_z;
} mpuData_t;

// MPU6050 偏航角结构体
typedef struct {
    s16 yaw_x;
    s16 yaw_y;
    s16 yaw_z;
} mpuYaw_t;

/* Global Variables -------------------------------------------------------- */

extern mpuData_t mpuData;
extern mpuYaw_t mpuYaw;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void mpu_gpio_init(u8 mpuNum);
void mpu_init(void);

// 功能函数

void mpu_write_reg(u8 addr, u8 data);
u8 mpu_read_reg(u8 addr);

u8 mpu_read_id(void);
void mpu_read_data(mpuData_t* mpuData);

// 算法函数

void mpu_yaw_core(const mpuData_t* src, mpuYaw_t* yaw);

#endif
