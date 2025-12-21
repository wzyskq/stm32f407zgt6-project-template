#include "sys.h"

/* Global Variables -------------------------------------------------------- */

u32 sysTime = 0;

s16 pwm[2]    = {0};
s16 spd[2]    = {0};
s16 whlCnt[2] = {0};

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

        oled_printf(0, 16 * 0, OLED_8X16, "%4d |%5d", pwm[0], whlCnt[0]);
        oled_printf(0, 16 * 1, OLED_8X16, "%4d |%5d", pwm[1], whlCnt[1]);

        oled_printf(8 * 4, 16 * 3, OLED_8X16, "%9d.%02d", sysTime / 100, sysTime % 100);
        oled_update();
    }
}
