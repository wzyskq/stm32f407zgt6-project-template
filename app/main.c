#include "main.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  主函数
 */
int main(void)
{
    /* NVIC中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 抢占0~3, 响应0~3

    serial_init(1, 115200, 0);
    // serial_init(2, 115200, 1);
    // serial_init(4, 115200, 2);

    mpu_init();

    oled_init();
    grays_init();
    keys_init();
    wheels_init();

    // PID 初始化
    pid_init(&pidValue[pidObj_whlLt], 0.7, 0.07, 0.05);
    pid_init(&pidValue[pidObj_whlRt], 0.7, 0.07, 0.05);
    pid_init(&pidValue[pidObj_pos], 5, 0, 0);
    pid_init(&pidValue[pidObj_dir], 10, 0, 0);

    // 定时器初始化
    timer_init(timObj_timer, 7, 0, 1000, 840, 0);   // 定时器模式 TIM7 无通道 10ms 响应优先级0
    timer_init(timObj_pwmOut, 1, 34, 1000, 840, 1); // PWM 输出模式 TIM1 通道3/4 10ms 响应优先级1
    timer_init(timObj_encoder, 3, 12, 0, 0, 2);     // 编码器模式 TIM3 通道1/2 10ms 响应优先级2
    timer_init(timObj_encoder, 4, 12, 0, 0, 2);     // 编码器模式 TIM4 通道1/2 10ms 响应优先级2

    // serial_printf(1, "1> System Init OK!\n");
    // serial_printf(2, "2> System Init OK!\n");
    // serial_printf(4, "4> System Init OK!\n");

    // wheel_pwm_set(1, 100);
    // wheel_pwm_set(2, 100);

    // oled_printf(0, 0, OLED_8X16, "你好");

    // Emm_V5_Vel_Control(4, 1, 0, 60, 00, false);
    // Emm_V5_Pos_Control(4, 1, 0, 60, 00, 16, true, false);

    sdc_init();
    if (mounted == FR_OK) {
        u32 allCapacity, freeCapacity;
        serial_printf(1, "FatFs:  SD Card Init Success.\n");
        sdc_get(&allCapacity, &freeCapacity, MB);
        serial_printf(1, "FatFs:  Available space: %d / %d MB\n", allCapacity, freeCapacity);
    }

    if (mounted == FR_OK){
        sdc_open("demo.txt");
        sdc_read(READ_FILE, 0);
        serial_printf(1, "FatFs:  File content:\n%s\n", sdcBuf);
    }

    loop();
}
