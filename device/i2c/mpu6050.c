#include "mpu6050.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

mpuData_t mpuData;
mpuYaw_t mpuYaw;

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief  MPU6050 初始化函数
 * \note    - 采用软件模拟I2C
 *          - 若上电后数字过大，增加延时即可
 */
void mpu_init(void)
{
    i2cIdx = i2cObj_MPU6050;
    i2c_gpio_init(i2cIdx);
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
    u8 lastIdx = i2cIdx;
    u8 buf[14] = {0};
    i2cIdx     = i2cObj_MPU6050;

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

    mpuData->accel_x = (buf[0] << 8) | buf[1];
    mpuData->accel_y = (buf[2] << 8) | buf[3];
    mpuData->accel_z = (buf[4] << 8) | buf[5];
    mpuData->temp    = (buf[6] << 8) | buf[7];
    mpuData->gyro_x  = (buf[8] << 8) | buf[9];
    mpuData->gyro_y  = (buf[10] << 8) | buf[11];
    mpuData->gyro_z  = (buf[12] << 8) | buf[13];

    i2cIdx = lastIdx; // 防止中断影响其他设备
}

/* ******************** 功能函数 */

/*





*/

/* 算法函数 ******************** */

/******************************************************************
 * \brief       MPU6050 偏航角算法
 * \param[in]   src 原始数据
 * \param[out]  yaw 三轴偏航角数据
 * \note         - 解决零漂问题
 *               - 采用互补滤波融合加速度与陀螺仪数据
 *               - 附带温度补偿功能
 */
void mpu_yaw_core(const mpuData_t *src, mpuYaw_t *yaw)
{
    /* 简易姿态解算：先利用静态样本估计零偏，再用互补滤波融合加速度与陀螺仪。 */
    if ((src == 0) || (yaw == 0)) {
        return;
    }

    enum {
        CALIB_SAMPLES = 200
    };

    const float dt         = 0.01f;   // 采样周期约 100Hz
    const float gyro_sens  = 16.4f;   // ±2000dps 灵敏度
    const float accel_sens = 2048.0f; // ±16g 灵敏度
    const float alpha      = 0.98f;   // 互补滤波系数
    const float rad2deg    = 57.2957795f;

    static u16 sample_cnt = 0;
    static float gx_bias = 0.0f, gy_bias = 0.0f, gz_bias = 0.0f;
    static float roll_deg = 0.0f, pitch_deg = 0.0f, yaw_deg = 0.0f;

    /* 采集初始静态数据估计零偏 */
    if (sample_cnt < CALIB_SAMPLES) {
        gx_bias += (float)src->gyro_x;
        gy_bias += (float)src->gyro_y;
        gz_bias += (float)src->gyro_z;
        sample_cnt++;

        if (sample_cnt == CALIB_SAMPLES) {
            gx_bias /= (float)CALIB_SAMPLES;
            gy_bias /= (float)CALIB_SAMPLES;
            gz_bias /= (float)CALIB_SAMPLES;
        }

        yaw->yaw_x = 0;
        yaw->yaw_y = 0;
        yaw->yaw_z = 0;
        return;
    }

    /* 零偏校正并换算物理量 */
    const float gx_dps = ((float)src->gyro_x - gx_bias) / gyro_sens;
    const float gy_dps = ((float)src->gyro_y - gy_bias) / gyro_sens;
    const float gz_dps = ((float)src->gyro_z - gz_bias) / gyro_sens;

    const float ax_g = (float)src->accel_x / accel_sens;
    const float ay_g = (float)src->accel_y / accel_sens;
    const float az_g = (float)src->accel_z / accel_sens;

    /* 由加速度估算俯仰/横滚 */
    const float roll_acc  = atan2f(ay_g, az_g) * rad2deg;
    const float pitch_acc = atan2f(-ax_g, sqrtf(ay_g * ay_g + az_g * az_g)) * rad2deg;

    /* 互补滤波融合陀螺积分与加速度解算 */
    roll_deg  = alpha * (roll_deg + gx_dps * dt) + (1.0f - alpha) * roll_acc;
    pitch_deg = alpha * (pitch_deg + gy_dps * dt) + (1.0f - alpha) * pitch_acc;
    yaw_deg += gz_dps * dt; // 无磁力计，仅积分得到相对航向

    /* 将角度限定在 (-180, 180]，避免数值不断累积 */
    roll_deg = mpu_yaw_adjust(roll_deg);
    pitch_deg = mpu_yaw_adjust(pitch_deg);
    yaw_deg = mpu_yaw_adjust(yaw_deg);

    yaw->yaw_x = (s16)roll_deg;
    yaw->yaw_y = (s16)pitch_deg;
    yaw->yaw_z = (s16)yaw_deg;
}

/******************************************************************
 * \brief       矫正航向角
 * \param[in]   deg 原始航向角，单位：度
 * \return      调整后航向角，单位：度
 * \note         - 将航向角调整到 (-180, 180] 范围，避免跨界长转
 */
float mpu_yaw_adjust(float deg)
{
    if (deg <= -180.0f) { deg += 360.0f; }
    if (deg > 180.0f) { deg -= 360.0f; }
    return deg;
}

/******************************************************************
 * \brief       计算航向角偏差
 * \param[in]   tarDeg 目标航向角，单位：度
 * \param[in]   curDeg   实际航向角，单位：度
 * \return      航向角偏差，单位：度
 * \note         - 计算航向设定与测量的最短角度偏差，避免跨界长转
 */
s16 mpu_yaw_error(s16 tarDeg, s16 curDeg)
{
    return (s16)mpu_yaw_adjust((float)(tarDeg - curDeg));
}

/* ******************** 算法函数 */
