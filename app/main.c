#include "main.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/******************************************************************
 * \brief  主函数
 */
int main(void)
{
    /* NVIC中断分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 抢占0..3, 响应0..3

    leds_init();
    keys_init();
    oled_init();

    mpu_init();
    grays_init();
    wheels_init();

    serial_init(usart1, 115200, 31);
    // serial_init(usart2, 115200, 1);
    serial_init(uart4, 115200, 01);

    pid_init(&pidValue[pidObj_whlLt], 0.7, 0.07, 0.05);
    pid_init(&pidValue[pidObj_whlRt], 0.7, 0.07, 0.05);
    pid_init(&pidValue[pidObj_pos], 5, 0, 0);
    pid_init(&pidValue[pidObj_dir], 10, 0, 0);

    timer_init(timMode_timer, tim6, 100, 5 * FRQ, 00);    // TIM6, 0.5ms
    timer_init(timMode_timer, tim7, 10000, 1 * FRQ, 11);  // TIM7, 10ms
    timer_init(timMode_pwmOut, tim1, 10000, 2 * FRQ, 33); // TIM1, 50Hz PWM 输出模式
    timer_init(timMode_encoder, tim3, 0, 0, 33);          // TIM3, 编码器模式
    timer_init(timMode_encoder, tim4, 0, 0, 33);          // TIM4, 编码器模式

    serial_printf(SRL_RESRL, "1> System Init OK!\n");
    // serial_printf(2, "2> System Init OK!\n");
    // serial_printf(4, "4> System Init OK!\n");

    // wheel_pwm_set(wheelLt, 100);
    // wheel_pwm_set(wheelRt, 100);

    // oled_printf(0, 0, OLED_8X16, "你好");

    // Emm_V5_Vel_Control(4, 1, 0, 60, 00, false);
    // Emm_V5_Pos_Control(4, 1, 0, 60, 00, 16, true, false);

    sdc_init();
    if (mounted == FR_OK) {
        u32 allCapacity, freeCapacity;
        serial_printf(SRL_RESRL, "FatFs:  SD Card Init Success.\n");
        sdc_get(&allCapacity, &freeCapacity, MB);
        serial_printf(SRL_RESRL, "FatFs:  Available space: %d / %d MB\n", allCapacity, freeCapacity);
    }

    // if (mounted == FR_OK){
    //     sdc_open("demo.txt");
    //     sdc_read(sdc_rFile, 0);
    //     sdc_open("1/demo.txt");
    //     sdc_read(sdc_rFile, 0);
    //     sdc_open("main.txt");
    //     sdc_write("Hello, 测试.\r\n");
    // }

    loop();
}
