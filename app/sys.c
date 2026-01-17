#include "sys.h"

/* Global Variables -------------------------------------------------------- */

u32 sysTime    = 0;
u8 whlTime     = 0;
u8 oledViewIdx = 3; // OLED 界面索引

// 巡线
s16 lineSpd = 0; // 巡线速度

// 方向环

// 位置环
u8 gray[9] = {0}; // 滤波灰度值
s8 grayVal = 0;   // 量化灰度值

// 速度环
s16 whlSpd[2] = {0}; // 目标速度
s16 whlCnt[2] = {0}; // 实际速度
s16 whlPwm[2] = {0}; // 实际输出

u8 whlStepDiv = 5;   // 步进段数
u8 whlStepErr = 150; // 最小误差
u8 whlStepExp = 6;   // 步进指数

// 临时调试变量
u8 tWhlS   = 0; // 时刻状态变量
s16 tWhlVn = 0; // 时刻速度变量

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  主循环
 */
void loop(void)
{
    while (1) {
        serial_decode_packet();
        serial_decode_sign();
        serial_decode_cmd();
        serial_decode_pid();

        key_judge(); // 按键检测

        if (taskNum == 1) {
            oledViewIdx++;
            if (oledViewIdx > 3) oledViewIdx = 0;
            taskNum = 0;
        } else if (taskNum == 2) {
        } else if (taskNum == 3) {
        } else if (taskNum == 4) {
        } else if (taskNum == 5) {
            if (!oledViewIdx)
                oledViewIdx = 3;
            else
                oledViewIdx--;
            taskNum = 0;
        } else if (taskNum == 6) {
        }

        // oled_printf(0, 16 * 0, OLED_8X16, "%4d |%5d", whlSpd[0], whlCnt[0]);
        // oled_printf(0, 16 * 1, OLED_8X16, "%4d |%5d", whlSpd[1], whlCnt[1]);

        oled_ui();
        // oled_printf(8 * 4, 16 * 3, OLED_8X16, "%9d.%02d", sysTime / 100, sysTime % 100);
        oled_update();
    }
}

/******************************************************************
 * \brief  速度环
 */
void speed_loop(void)
{
    // 获取编码器速度
    whlCnt[0] = timer_encoder_read(5, normal);
    whlCnt[1] = timer_encoder_read(4, inverse);

    // 指数步进实现
    static s16 Vn[2] = {0}; // 下一步目标速度
    Vn[0]            = exp_step_core(0, whlStepDiv, whlStepExp, whlStepErr, whlSpd[0], whlCnt[0]);
    Vn[1]            = exp_step_core(1, whlStepDiv, whlStepExp, whlStepErr, whlSpd[1], whlCnt[1]);

    // 速度环计算
    /*
        pidIdx                    = 1;
        pidValue[pidIdx].setpoint = (float)whlSpd[0];
        neural_pid_action(&pidValue[pidIdx], (float)whlCnt[0]);
        whlPwm[0] = (s16)pidValue[pidIdx].result;
        pidIdx                    = 2;
        pidValue[pidIdx].setpoint = (float)whlSpd[1];
        neural_pid_action(&pidValue[pidIdx], (float)whlCnt[1]);
        whlPwm[1] = (s16)pidValue[pidIdx].result;
    */
    pidIdx    = 1;
    whlPwm[0] = (s16)pid_action(&pidValue[pidIdx], (float)Vn[0], (float)whlCnt[0]);

    pidIdx    = 2;
    whlPwm[1] = (s16)pid_action(&pidValue[pidIdx], (float)Vn[1], (float)whlCnt[1]);

    // 最低限幅速度
    if (-100 < whlPwm[0] && whlPwm[0] < 100) whlPwm[0] = 0;
    if (-100 < whlPwm[1] && whlPwm[1] < 100) whlPwm[1] = 0;

    // PWM 调速
    wheel_pwm_set(1, whlPwm[0]);
    wheel_pwm_set(2, whlPwm[1]);
}

/******************************************************************
 * \brief  位置环
 */
void position_loop(void)
{
    // 灰度传感器检测与滤波
    gray_scan();
    grayVal = gray_quant_core(graySrc, gray);
}

/******************************************************************
 * \brief  方向环
 */
void direction_loop(void)
{
    // MPU6050 数据读取
    mpu_read_data(&mpuData);

    // 偏航角计算
    mpu_yaw_core(&mpuData, &mpuYaw);
}

/******************************************************************
 * \brief  OLED UI
 */
void oled_ui(void)
{
    oled_clear_area(0, 0, 8 * 4, 16 * 3);
    oled_clear_area(8 * 4, 0, 8 * 12, 16 * 4);

    if (oledViewIdx == 0) {
        // wheel
        oled_draw_line(1, 1, 14, 1);
        oled_draw_line(2, 2, 6, 2);
        oled_draw_line(9, 2, 13, 2);

        oled_draw_rectangle(2, 4, 5, (u8)abs16(whlPwm[0] * 40 / 1000), OLED_FILLED);
        oled_draw_rectangle(9, 4, 5, (u8)abs16(whlPwm[1] * 40 / 1000), OLED_FILLED);

        oled_draw_line(2, 45, 6, 45);
        oled_draw_line(9, 45, 13, 45);
        oled_draw_line(1, 46, 14, 46);

        // oled_printf(0, 8 * 6, OLED_6X8, "pL %d %d", (u8)abs16(whlPwm[0]*40/1000), (whlPwm[0]*40/1000));
        // oled_printf(0, 8 * 7, OLED_6X8, "pR %d %d", (u8)abs16(whlPwm[1]*40/1000), (whlPwm[1]*40/1000));

        oled_printf(8 * 2, 8 * 0, OLED_6X8, "rL %d", whlCnt[0]);
        oled_printf(8 * 2, 8 * 1, OLED_6X8, "rR %d", whlCnt[1]);
        oled_printf(8 * 2, 16 * 1, OLED_8X16, "L %d", whlSpd[0]);
        oled_printf(8 * 2, 16 * 2, OLED_8X16, "R %d", whlSpd[1]);

        oled_printf(8 * 9, 8 * 0, OLED_6X8, "wDiv %d", whlStepDiv);
        oled_printf(8 * 9, 8 * 2, OLED_6X8, "wErr %d", whlStepErr);
        oled_printf(8 * 9, 8 * 4, OLED_6X8, "wExp %d", whlStepExp);

    } else if (oledViewIdx == 1) {
        // pidc
        pidIdx = 1;
        oled_printf(74, 8 * 0, OLED_6X8, "P1 %g", pidValue[pidIdx].kp);
        oled_printf(74, 8 * 1, OLED_6X8, "I1 %g", pidValue[pidIdx].ki);
        oled_printf(74, 8 * 2, OLED_6X8, "D1 %g", pidValue[pidIdx].kd);
        // oled_printf(74, 8 * 3, OLED_6X8, "C1 %g", pidValue[pidIdx].kcoef);

        pidIdx = 2;
        oled_printf(74, 8 * 4, OLED_6X8, "P2 %g", pidValue[pidIdx].kp);
        oled_printf(74, 8 * 5, OLED_6X8, "I2 %g", pidValue[pidIdx].ki);
        oled_printf(74, 8 * 6, OLED_6X8, "D2 %g", pidValue[pidIdx].kd);
        // oled_printf(74, 8 * 7, OLED_6X8, "C2 %g", pidValue[pidIdx].kcoef);
    }

    else if (oledViewIdx == 2) {
        // gray
        oled_printf(0, 16 * 0, OLED_8X16, "Gray: %d", grayVal);
        // oled_printf(0, 16 * 0, OLED_8X16, "%d %d %d %d  %d %d %d %d",
        //             graySrc[1], graySrc[2], graySrc[3], graySrc[4],
        //             graySrc[5], graySrc[6], graySrc[7], graySrc[8]);
        oled_printf(0, 16 * 1, OLED_8X16, "%d %d %d %d  %d %d %d %d",
                    gray[1], gray[2], gray[3], gray[4],
                    gray[5], gray[6], gray[7], gray[8]);
        // oled_printf(0, 16 * 2, OLED_8X16, "0x%x", mpu_read_id());
    }

    else if (oledViewIdx == 3) {
        // mpu6050
        oled_printf(8 * 0, 8 * 0, OLED_6X8, "%+d", mpuData.accel_x);
        oled_printf(8 * 0, 8 * 1, OLED_6X8, "%+d", mpuData.accel_y);
        oled_printf(8 * 0, 8 * 2, OLED_6X8, "%+d", mpuData.accel_z);

        oled_printf(8 * 0, 8 * 3, OLED_6X8, "%+d", mpuData.gyro_x);
        oled_printf(8 * 0, 8 * 4, OLED_6X8, "%+d", mpuData.gyro_y);
        oled_printf(8 * 0, 8 * 5, OLED_6X8, "%+d", mpuData.gyro_z);

        oled_printf(8 * 8, 16 * 3, OLED_8X16, "%+d", mpuData.temp);

        oled_printf(8 * 8, 16 * 0, OLED_8X16, "%+d", mpuYaw.yaw_x);
        oled_printf(8 * 8, 16 * 1, OLED_8X16, "%+d", mpuYaw.yaw_y);
        oled_printf(8 * 8, 16 * 2, OLED_8X16, "%+d", mpuYaw.yaw_z);
    }
}
