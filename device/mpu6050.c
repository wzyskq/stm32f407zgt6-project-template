#include "mpu6050.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const u32 mpuRccGpio[] = {
    0,
    RCC_AHB1Periph_GPIOB,
};

static GPIO_TypeDef *mpuGpioPort[] = {
    0,
    GPIOB,
};

static const u16 mpuGpioPin[] = {
    0,
    GPIO_Pin_15 | GPIO_Pin_14,
};

/* Global Variables -------------------------------------------------------- */

mpuData_t mpuData;

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      MPU6050 GPIO 初始化
 * \param[in]  mpuNum
 */
void mpu_gpio_init(u8 mpuNum)
{
    /* 使能 GPIO 时钟 */
    RCC_AHB1PeriphClockCmd(mpuRccGpio[mpuNum], ENABLE);

    /* I2C 引脚配置 */
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin   = mpuGpioPin[mpuNum];
    GPIO_Init(mpuGpioPort[mpuNum], &GPIO_InitStruct);

    /* 设置引脚初始状态 */
    i2c_w_SCL(1);
    i2c_w_SDA(1);
}

/******************************************************************
 * \brief  MPU6050 初始化函数
 * \note    - 采用软件模拟I2C
 *          - 若上电后数字过大，增加延时即可
 */
void mpu_init(void)
{
    i2cIdx = i2cObj_MPU6050;
    mpu_gpio_init(1);
    delay_ms(8); // 等待器件稳定

    mpu_write_reg(MPU6050_PWR_MGMT_1, 0x01);   // 解除睡眠 选择陀螺仪X轴为时钟源
    mpu_write_reg(MPU6050_PWR_MGMT_2, 0x00);   // 启用所有轴
    mpu_write_reg(MPU6050_SMPLRT_DIV, 0x09);   // 设置采样率 1kHz/(1+9)=100Hz
    mpu_write_reg(MPU6050_CONFIG, 0x06);       // 低通滤波频率 5Hz
    mpu_write_reg(MPU6050_GYRO_CONFIG, 0x18);  // 陀螺仪最大量程 ±2000°/s
    mpu_write_reg(MPU6050_ACCEL_CONFIG, 0x18); // 加速度计最大量程 ±16g
}
/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief      MPU6050 写入一位寄存器
 * \param[in]  addr 寄存器地址
 * \param[in]  data 写入数据
 */
void mpu_write_reg(u8 addr, u8 data)
{
    i2c_start();
    i2c_send_byte(0xD0); // 写地址
    i2c_recv_ack();
    i2c_send_byte(addr); // 寄存器地址
    i2c_recv_ack();

    i2c_send_byte(data); // 写数据
    i2c_recv_ack();
    i2c_stop();
}

/******************************************************************
 * \brief      MPU6050 读取一位寄存器
 * \param[in]  addr 寄存器地址
 * \return     读取数据
 */
u8 mpu_read_reg(u8 addr)
{
    u8 data;

    i2c_start();
    i2c_send_byte(0xD0); // 写地址
    i2c_recv_ack();
    i2c_send_byte(addr); // 寄存器地址
    i2c_recv_ack();

    i2c_start();         // 重复起始信号
    i2c_send_byte(0xD1); // 发送从机地址，读写位为1（即 0xD0 | 0x01）
    i2c_recv_ack();
    data = i2c_recv_byte(); // 读数据
    i2c_send_ack(1);        // NACK
    i2c_stop();

    return data;
}

/******************************************************************
 * \brief   MPU6050 读取设备ID
 * \return  设备ID
 */
u8 mpu_read_id(void)
{
    return mpu_read_reg(MPU6050_WHO_AM_I);
}

/******************************************************************
 * \brief       MPU6050 读取所有传感器数据
 * \param[out]  mpuData 指向 mpuData_t 结构体的指针
 */
void mpu_read_data(mpuData_t *mpuData)
{
    i2cIdx = i2cObj_MPU6050;
    u8 buf[14] = {0};

    i2c_start();
    i2c_send_byte(0xD0); // 写地址
    i2c_recv_ack();

    i2c_send_byte(MPU6050_ACCEL_XOUT_H); // 寄存器地址
    i2c_recv_ack();

    i2c_start();         // 重复起始信号
    i2c_send_byte(0xD1); // 发送从机地址，读写位为1（即 0xD0 | 0x01）
    i2c_recv_ack();

    for (u8 i = 0; i < 14; i++) {
        buf[i] = i2c_recv_byte(); // 读数据
        if (i < 13) {
            i2c_send_ack(0); // ACK
        } else {
            i2c_send_ack(1); // NACK
        }
    }
    i2c_stop();

    mpuData->Accel_X = (buf[0] << 8) | buf[1];
    mpuData->Accel_Y = (buf[2] << 8) | buf[3];
    mpuData->Accel_Z = (buf[4] << 8) | buf[5];
    mpuData->Temp    = (buf[6] << 8) | buf[7];
    mpuData->Gyro_X  = (buf[8] << 8) | buf[9];
    mpuData->Gyro_Y  = (buf[10] << 8) | buf[11];
    mpuData->Gyro_Z  = (buf[12] << 8) | buf[13];
}

/* ******************** 功能函数 */
