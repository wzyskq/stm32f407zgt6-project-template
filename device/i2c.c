#include "mpu6050.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static GPIO_TypeDef *i2cGpioPort[][2] = {
    // SCL, SDA
    {0, 0},
    {GPIOB, GPIOB}, // mpu6050
};

static const u16 i2cGpioPin[][2] = {
    // SCL, SDA
    {0, 0},
    {GPIO_Pin_15, GPIO_Pin_14}, // mpu6050
};

/* Global Variables -------------------------------------------------------- */

volatile u8 i2cIdx = 0;  // I2C 对象索引

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 通信工具 ******************** */

/******************************************************************
 * \brief      I2C 延时函数
 * \param[in]  delay 延时参数，0..65535
 */
void i2c_delay(u16 delay)
{
    delay *= 20;
    while (delay--)
        __NOP(); // 空操作，延时
}

/******************************************************************
 * \brief      I2C 写入 SCL 引脚
 * \param[in]  bit 写入值，0/1
 */
void i2c_w_SCL(u8 bit)
{
    GPIO_WriteBit(i2cGpioPort[i2cIdx][0], i2cGpioPin[i2cIdx][0], (BitAction)(bit));
    i2c_delay(1);
}

/******************************************************************
 * \brief      I2C 写入 SDA 引脚
 * \param[in]  bit 写入值，0/1
 */
void i2c_w_SDA(u8 bit)
{
    GPIO_WriteBit(i2cGpioPort[i2cIdx][1], i2cGpioPin[i2cIdx][1], (BitAction)(bit));
    i2c_delay(1);
}

/******************************************************************
 * \brief      I2C 读取 SCL 引脚
 * \return     读取值，0/1
 */
u8 i2c_r_SCL(void)
{
    u8 bit = GPIO_ReadInputDataBit(i2cGpioPort[i2cIdx][0], i2cGpioPin[i2cIdx][0]);
    i2c_delay(1);
    return bit;
}

/******************************************************************
 * \brief      I2C 读取 SDA 引脚
 * \return     读取值，0/1
 */
u8 i2c_r_SDA(void)
{
    u8 bit = GPIO_ReadInputDataBit(i2cGpioPort[i2cIdx][1], i2cGpioPin[i2cIdx][1]);
    i2c_delay(1);
    return bit;
}

/* ******************** 通信工具 */

/*





*/

/* 通信协议 ******************** */

/******************************************************************
 * \brief      I2C 起始信号
 */
void i2c_start(void)
{
    i2c_w_SDA(1);
    i2c_w_SCL(1);
    i2c_w_SDA(0);
    i2c_w_SCL(0);
}

/******************************************************************
 * \brief      I2C 终止信号
 */
void i2c_stop(void)
{
    i2c_w_SDA(0);
    i2c_w_SCL(1);
    i2c_w_SDA(1);
}

/******************************************************************
 * \brief      I2C 发送一个字节
 * \param[in]  byte 要发送的字节
 */
void i2c_send_byte(u8 byte)
{
    for (u8 i = 0; i < 8; i++) {
        i2c_w_SDA((byte & (0x80 >> i)));
        i2c_w_SCL(1);
        i2c_w_SCL(0);
    }
}

/******************************************************************
 * \brief      I2C 接收一个字节
 * \return     接收到的字节
 */
u8 i2c_recv_byte(void)
{
    u8 byte = 0x00;
    i2c_w_SDA(1); // 释放 SDA 总线
    for (u8 i = 0; i < 8; i++) {
        i2c_w_SCL(1);
        if (i2c_r_SDA()) byte |= (0x80 >> i);
        i2c_w_SCL(0);
    }
    return byte;
}

/******************************************************************
 * \brief      I2C 发送应答信号
 * \param[in]  ack 应答信号，0：ACK；1：NACK
 */
void i2c_send_ack(u8 ack)
{
    i2c_w_SDA(ack);
    i2c_w_SCL(1);
    i2c_w_SCL(0);
}

/******************************************************************
 * \brief      I2C 接收应答信号
 * \return     应答信号，0：ACK；1：NACK
 */
u8 i2c_recv_ack(void)
{
    u8 ack;
    i2c_w_SDA(1); // 释放 SDA 总线
    i2c_w_SCL(1);
    ack = i2c_r_SDA();
    i2c_w_SCL(0);
    return ack;
}

/* ******************** 通信协议 */
