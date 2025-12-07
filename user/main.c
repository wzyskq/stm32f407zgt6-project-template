#include "main.h"

/* Global Variables -------------------------------------------------------- */

// 定时中断
u8 serialTimeFlag = 0; // MCU 运行时间标志位
u16 serialTime    = 0; // 运行时间 单位：10ms

// 按键
u16 keyBox[] = {0, 0, 0}; // 按键时间 {按键编号, 按键次数, 按键时间} 单位：10ms
u8 taskNum   = 0;         // 当前按键

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  主函数
 */
int main(void)
{
    serial_init(1, 115200, 0);

    // gpio_init_pa2();
    key_init();
    oled_init();

    // pid_init(&pidValue[0], 0.5, 0.1, 0.05);
    // pid_init(&pidValue[1], 0.2, 0.1, 0.05);

    // pwm_init(1000, 720);
    // rotate_init();

    timer_init_2();
    serial_printf(USART1, "System Init OK!\n");
    oled_printf(0, 0, OLED_8X16, "你好");
    // oled_update();

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

        oled_update();
    }
}

// 定时中断触发 运行时间 10ms
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
        // 串口
        if (serialTimeFlag)
            serialTime++;

        // 按键
        if (keyBox[2])
            keyBox[2]++;
        if (keyBox[2] == 100) { // 1s 后检查
            key_action();
            keyBox[2] = 0; // 关闭自增
        }

        // pass

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
