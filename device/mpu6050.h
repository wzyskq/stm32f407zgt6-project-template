/**
 * \page  MPU6050 六轴传感器驱动代码说明
 *   \li  参考链接： https://blog.csdn.net/m0_74082999/article/details/155985159
 */
#ifndef __MPU6050_H_
#define __MPU6050_H_

#include "main.h"
#include "mpu6050_reg.h"

/* Global Types ------------------------------------------------------------ */

typedef struct {
    s16 Accel_X;
    s16 Accel_Y;
    s16 Accel_Z;
    s16 Temp;
    s16 Gyro_X;
    s16 Gyro_Y;
    s16 Gyro_Z;
} mpuData_t;

/* Global Variables -------------------------------------------------------- */

extern mpuData_t mpuData;

/* Global Functions -------------------------------------------------------- */

// 初始化函数

void mpu_gpio_init(u8 mpuNum);
void mpu_init(void);

// 功能函数

void mpu_write_reg(u8 addr, u8 data);
u8 mpu_read_reg(u8 addr);

u8 mpu_read_id(void);
void mpu_read_data(mpuData_t* mpuData);

#endif
