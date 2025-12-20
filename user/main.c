#include "main.h"

/* Global Variables -------------------------------------------------------- */

u32 oledTime = 0;

s16 pwm[2]    = {0};
s16 spd[2]    = {0};
s16 whlCnt[2] = {0};

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  主函数
 */
int main(void)
{
    /* NVIC中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 抢占0~3, 响应0~3

    serial_init(1, 115200, 0);

    // gpio_init_pa2();

    oled_init();
    keys_init();
    wheels_init();

    // pid_init(&pidValue[0], 0.5, 0.1, 0.05);
    // pid_init(&pidValue[1], 0.2, 0.1, 0.05);

    // pwm_init(1000, 720);
    // rotate_init();

    timer_init(timer, 7, 0, 1000, 840, 0);   // 定时器模式 TIM7 无通道 10ms 响应优先级0
    timer_init(pwmOut, 1, 34, 1000, 840, 1); // PWM 输出模式 TIM1 通道3/4 10ms 响应优先级1
    timer_init(encoder, 5, 12, 0, 0, 2);     // 编码器模式 TIM5 通道1/2 10ms 响应优先级2
    timer_init(encoder, 4, 12, 0, 0, 2);     // 编码器模式 TIM4 通道1/2 10ms 响应优先级2

    serial_printf(USART1, "System Init OK!\n");

    // wheel_pwm_set(1, 100);
    // wheel_pwm_set(2, 100);

    // oled_printf(0, 0, OLED_8X16, "你好");

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

        oled_printf(8 * 4, 16 * 3, OLED_8X16, "%9d.%02d", oledTime / 100, oledTime % 100);
        oled_update();
    }
}

// 定时中断触发 运行时间 10ms
void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET) {
        // 串口
        // if (serialTimeFlag)
        //     serialTime++;

        // 按键
        if (keyBox[2])
            keyBox[2]++;
        if (keyBox[2] == 100) { // 1s 后检查
            key_action();
            keyBox[2] = 0; // 关闭自增
        }

        // OLED 刷新
        oledTime++;

        // 获取编码器速度
        whlCnt[0] = timer_encoder_read(5, normal);
        whlCnt[1] = timer_encoder_read(4, inverse);

        // PWM 调速
        wheel_pwm_set(1, pwm[0]);
        wheel_pwm_set(2, pwm[1]);

        // pass

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}
