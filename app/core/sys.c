#include "sys.h"

/* Global Variables -------------------------------------------------------- */

u32 sysTime    = 0;
u8 oledViewIdx = 0; // OLED 界面索引

// 循迹
u8 trkMode = 0; // 循迹模式
s16 trkSpd = 0; // 循迹速度

// 方向环
s16 carDeg = 0; // 目标偏航角

// 位置环
u8 gray[9] = {0}; // 滤波灰度值
s8 grayVal = 0;   // 量化灰度值

// 速度环
s16 whlSpd[2] = {0}; // 目标速度
s16 whlCnt[2] = {0}; // 实际速度
s16 whlPwm[2] = {0}; // 实际输出

u8 spdStepDiv = 5;   // 步进段数
u8 spdStepErr = 150; // 最小误差
u8 spdStepExp = 6;   // 步进指数

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
            taskNum = 101;
        } else if (taskNum == 2) {
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Read_Sys_Params(ZDT_SRL, 1, S_VEL);
            while (zdtTvFlg); // 等待数据
            serial_printf(1, "v = %d rpm\n", (s32)zdtSysData.vel);

        } else if (taskNum == 3) {
            // while (zdtTvFlg); // 等待清零
            Emm_V5_Read_Sys_Params(ZDT_SRL, 1, S_CPOS);
            while (zdtTvFlg); // 等待数据
            serial_printf(1, "cpos = %.2f deg\n", zdtSysData.cpos);

        } else if (taskNum == 4) {
        } else if (taskNum == 5) {
            oledViewIdx++;
            if (oledViewIdx > 4) oledViewIdx = 0;
            taskNum = 0;
        } else if (taskNum == 6) {
            if (!oledViewIdx)
                oledViewIdx = 4;
            else
                oledViewIdx--;
            taskNum = 0;
        }

        if (taskNum == 101) {
            demo_1();
        } else if (taskNum == 102) {
            demo_2();
        }

        // oled_printf(0, 16 * 0, OLED_8X16, "%4d |%5d", whlSpd[0], whlCnt[0]);
        // oled_printf(0, 16 * 1, OLED_8X16, "%4d |%5d", whlSpd[1], whlCnt[1]);

        // 灰度传感器检测与滤波
        // gray_scan();
        // grayVal = gray_quant_core(graySrc, gray);

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
    whlCnt[0] = timer_encoder_readCnt(tim4, normal);
    whlCnt[1] = timer_encoder_readCnt(tim3, inverse);

    // 指数步进实现
    static s16 Vn[2] = {0}; // 下一步目标速度
    Vn[0]            = exp_step_core(expIdx_whlLt, spdStepDiv, spdStepExp, spdStepErr, whlSpd[0], whlCnt[0]);
    Vn[1]            = exp_step_core(expIdx_whlRt, spdStepDiv, spdStepExp, spdStepErr, whlSpd[1], whlCnt[1]);

    // 速度环计算
    pidIdx    = pidObj_whlLt;
    whlPwm[0] = (s16)pid_action(&pidValue[pidIdx], (float)Vn[0], (float)whlCnt[0]);

    pidIdx    = pidObj_whlRt;
    whlPwm[1] = (s16)pid_action(&pidValue[pidIdx], (float)Vn[1], (float)whlCnt[1]);

    // 最低限幅速度
    if (-100 < whlPwm[0] && whlPwm[0] < 100) whlPwm[0] = 0;
    if (-100 < whlPwm[1] && whlPwm[1] < 100) whlPwm[1] = 0;

    // PWM 调速
    wheel_pwm_set(wheelLt, whlPwm[0]);
    wheel_pwm_set(wheelRt, whlPwm[1]);
}

/******************************************************************
 * \brief  位置环
 */
void position_loop(void)
{
    // 灰度传感器检测与滤波
    gray_scan();
    grayVal = gray_quant_core(graySrc, gray);

    // 退出机制
    if (trkMode != trkObj_pos) return;

    // 位置环计算
    pidIdx   = pidObj_pos;
    s16 diff = (s16)pid_action(&pidValue[pidIdx], 0, grayVal);

    // 设定目标速度
    whlSpd[0] = trkSpd - diff;
    whlSpd[1] = trkSpd + diff;
}

/******************************************************************
 * \brief  方向环
 */
void direction_loop(void)
{
    // MPU6050 计算偏航角
    mpu_read_data(&mpuData);
    mpu_yaw_core(&mpuData, &mpuYaw);

    // 退出机制
    if (trkMode != trkObj_dir) return;

    // 方向环计算
    pidIdx   = pidObj_dir;
    s16 diff = (s16)pid_action(&pidValue[pidIdx], (float)carDeg, (float)mpuYaw.yaw_z);

    // 设定目标速度
    whlSpd[0] = trkSpd - diff;
    whlSpd[1] = trkSpd + diff;
}

/******************************************************************
 * \brief  OLED UI
 */
void oled_ui(void)
{
    oled_clear_area(0, 0, 8 * 4, 16 * 3);
    oled_clear_area(8 * 4, 0, 8 * 12, 16 * 4);

    if (oledViewIdx == 0) {
        oled_printf(8 * 0, 8 * 0, OLED_6X8, "trkMode %d", trkMode);
        oled_printf(8 * 0, 8 * 2, OLED_6X8, "trkSpd %d", trkSpd);
        oled_printf(8 * 0, 8 * 4, OLED_6X8, "TarL %d", whlSpd[0]);
        oled_printf(8 * 0, 8 * 5, OLED_6X8, "TarR %d", whlSpd[1]);

        oled_printf(8 * 9, 8 * 0, OLED_6X8, "Pos: %+d", grayVal);
        oled_printf(8 * 9, 8 * 2, OLED_6X8, "Dir: %+d", mpuYaw.yaw_z);
        oled_printf(8 * 9, 8 * 4, OLED_6X8, "Task: %d", taskNum);
        oled_printf(8 * 4, 8 * 6, OLED_8X16, "%9d.%02d", sysTime / 100, sysTime % 100);

    } else if (oledViewIdx == 1) {
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

        oled_printf(8 * 9, 8 * 0, OLED_6X8, "wDiv %d", spdStepDiv);
        oled_printf(8 * 9, 8 * 2, OLED_6X8, "wErr %d", spdStepErr);
        oled_printf(8 * 9, 8 * 4, OLED_6X8, "wExp %d", spdStepExp);
    } else if (oledViewIdx == 2) {
        // pidc
        pidIdx = pidObj_pos;
        oled_printf(0, 8 * 0, OLED_6X8, "P3 %g", pidValue[pidIdx].kp);
        oled_printf(0, 8 * 1, OLED_6X8, "I3 %g", pidValue[pidIdx].ki);
        oled_printf(0, 8 * 2, OLED_6X8, "D3 %g", pidValue[pidIdx].kd);
        oled_printf(0, 8 * 3, OLED_8X16, "%+d", grayVal);

        // pidIdx = pidObj_dir;
        // oled_printf(0, 8 * 0, OLED_6X8, "P4 %g", pidValue[pidIdx].kp);
        // oled_printf(0, 8 * 1, OLED_6X8, "I4 %g", pidValue[pidIdx].ki);
        // oled_printf(0, 8 * 2, OLED_6X8, "D4 %g", pidValue[pidIdx].kd);
        // oled_printf(0, 8 * 3, OLED_8X16, "%+d", mpuYaw.yaw_z);

        pidIdx = pidObj_whlLt;
        oled_printf(74, 8 * 0, OLED_6X8, "P1 %g", pidValue[pidIdx].kp);
        oled_printf(74, 8 * 1, OLED_6X8, "I1 %g", pidValue[pidIdx].ki);
        oled_printf(74, 8 * 2, OLED_6X8, "D1 %g", pidValue[pidIdx].kd);
        // oled_printf(74, 8 * 3, OLED_6X8, "C1 %g", pidValue[pidIdx].kcoef);

        pidIdx = pidObj_whlRt;
        oled_printf(74, 8 * 4, OLED_6X8, "P2 %g", pidValue[pidIdx].kp);
        oled_printf(74, 8 * 5, OLED_6X8, "I2 %g", pidValue[pidIdx].ki);
        oled_printf(74, 8 * 6, OLED_6X8, "D2 %g", pidValue[pidIdx].kd);
        // oled_printf(74, 8 * 7, OLED_6X8, "C2 %g", pidValue[pidIdx].kcoef);
    }

    else if (oledViewIdx == 3) {
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

    else if (oledViewIdx == 4) {
        // mpu6050
        oled_printf(8 * 0, 8 * 0, OLED_6X8, "A: %+d", mpuData.accel_x);
        oled_printf(8 * 0, 8 * 1, OLED_6X8, "A: %+d", mpuData.accel_y);
        oled_printf(8 * 0, 8 * 2, OLED_6X8, "A: %+d", mpuData.accel_z);

        oled_printf(8 * 0, 8 * 3, OLED_6X8, "G: %+d", mpuData.gyro_x);
        oled_printf(8 * 0, 8 * 4, OLED_6X8, "G: %+d", mpuData.gyro_y);
        oled_printf(8 * 0, 8 * 5, OLED_6X8, "G: %+d", mpuData.gyro_z);

        // oled_printf(8 * 8, 16 * 3, OLED_8X16, "%+d", mpuData.temp);
        oled_printf(8 * 4, 16 * 3, OLED_8X16, "%9d.%02d", sysTime / 100, sysTime % 100);

        oled_printf(8 * 8, 16 * 0, OLED_8X16, "X: %+d", mpuYaw.yaw_x);
        oled_printf(8 * 8, 16 * 1, OLED_8X16, "Y: %+d", mpuYaw.yaw_y);
        oled_printf(8 * 8, 16 * 2, OLED_8X16, "Z: %+d", mpuYaw.yaw_z);
    }
}
