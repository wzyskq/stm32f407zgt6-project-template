#include "sys.h"

/* Global Variables -------------------------------------------------------- */

u32 sysTime    = 0;
u8 oledViewIdx = 0;

u8 whlTime = 0;

s16 whlVn = 0; // 速度临时变量
u8 whlS   = 0; // 位置临时变量

s16 whlSpd[2] = {0}; // 目标速度
s16 whlCnt[2] = {0}; // 实际速度
s16 whlPwm[2] = {0}; // 实际输出

// 速度环
u8 whlStepNum    = 6; // 步进段数
u8 whlMinStepErr = 20; // 最小误差
u8 whlExponent   = 3; // 步进指数

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
        } else if (taskNum == 2) {
        } else if (taskNum == 3) {
        } else if (taskNum == 4) {
        } else if (taskNum == 5) {
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
    static s16 l[2]   = {0}; // 步进单位长度
    static s16 Vt0[2] = {0}; // 目标速度备份
    static s16 Vn[2]  = {0}; // 下一步目标速度
    static s16 s[2]   = {0}; // 步进状态

    static u8 sbsFlag[2] = {0}; // 步进标志位

    // 获取编码器速度
    whlCnt[0] = timer_encoder_read(5, normal);
    whlCnt[1] = timer_encoder_read(4, inverse);

    // 步进实现
    if (Vt0[0] != whlSpd[0]) {
        l[0]       = ceil((whlSpd[0] - whlCnt[0]) / pow(whlStepNum, whlExponent));
        Vt0[0]     = whlSpd[0];
        Vn[0]      = whlCnt[0];
        s[0]       = 0;
        sbsFlag[0] = 1;
    }
    if (sbsFlag[0]) {
        if (abs16(Vn[0] - whlCnt[0]) < whlMinStepErr * (whlStepNum + 1 - s[0])) {
            s[0]++;
            if (s[0] == whlStepNum) {
                Vn[0]      = whlSpd[0];
                sbsFlag[0] = 0;
            } else {
                if (whlExponent == 2)
                    Vn[0] += l[0] * (2 * whlStepNum - 2 * s[0] + 1);
                else if (whlExponent == 3) {
                    u8 k = whlStepNum - s[0];
                    Vn[0] += l[0] * (3 * k * k - 3 * k + 1);
                } else if (whlExponent == 4) {
                    u8 k = whlStepNum - s[0];
                    Vn[0] += l[0] * (4 * k * k * k - 6 * k * k + 4 * k + 1);
                }
            }
        }
    } else {
        Vn[0] = whlSpd[0];
    }

    if (Vt0[1] != whlSpd[1]) {
        l[1]       = ceil((whlSpd[1] - whlCnt[1]) / pow(whlStepNum, whlExponent));
        Vt0[1]     = whlSpd[1];
        Vn[1]      = whlCnt[1];
        s[1]       = 0;
        sbsFlag[1] = 1;
    }
    if (sbsFlag[1]) {
        if (abs16(Vn[1] - whlCnt[1]) < whlMinStepErr * (whlStepNum + 1 - s[1])) {
            s[1]++;
            if (s[1] == whlStepNum) {
                Vn[1]      = whlSpd[1];
                sbsFlag[1] = 0;
            } else {
                if (whlExponent == 2)
                    Vn[1] += l[1] * (2 * whlStepNum - 2 * s[1] + 1);
                else if (whlExponent == 3) {
                    u8 k = whlStepNum - s[1];
                    Vn[1] += l[1] * (3 * k * k - 3 * k + 1);
                } else if (whlExponent == 4) {
                    u8 k = whlStepNum - s[1];
                    Vn[1] += l[1] * (4 * k * k * k - 6 * k * k + 4 * k + 1);
                }
            }
        }
    } else {
        Vn[1] = whlSpd[1];
    }

    whlS  = s[0];
    whlVn = Vn[0];

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

        oled_printf(8 * 9, 8 * 0, OLED_6X8, "whlStp %d", whlStepNum);
        oled_printf(8 * 9, 8 * 2, OLED_6X8, "whlErr %d", whlMinStepErr);
        oled_printf(8 * 9, 8 * 4, OLED_6X8, "whlExp %d", whlExponent);

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
}
