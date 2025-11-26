#include "main.h"

// 定时中断
BYTE serialTimeFlag = 0; // MCU 运行时间标志位
WORD serialTime = 0;     // 运行时间 单位：10ms

// 按键
WORD keyBox[] = {0, 0, 0}; // 按键时间 {按键编号, 按键次数, 按键时间} 单位：10ms
BYTE keyFlag = 0;          // 当前按键

// PID
BYTE pidIdx = 0; // PID 索引
WORD pidErr = 0; // PID 误差

int main(void)
{
    // gpio_init_pa2();
    key_init();
    oled_init();

    // pid_init(&pidValue[0], 0.5, 0.1, 0.05);
    // pid_init(&pidValue[1], 0.2, 0.1, 0.05);

    // pwm_init(1000, 720);
    // rotate_init();

    serial_init_3(115200); // 视觉串口
    serial_init_1(115200); // 调试串口
    timer_init_2();

    oled_printf(0, 0, OLED_8X16, "你好");
    // oled_update();

    while (1)
    {
        serial_process_packet();
        serial_process_sign();
        serial_process_cmd();
        serial_process_pid();

        key_judge(); // 按键检测

        if (keyFlag == 1)
        {
        }
        else if (keyFlag == 2)
        {
        }
        else if (keyFlag == 3)
        {
        }
        else if (keyFlag == 4)
        {
        }
        else if (keyFlag == 5)
        {
        }
        else if (keyFlag == 6)
        {
        }

        oled_update();
    }
}

// 定时中断触发 运行时间 10ms
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        // 串口
        if (serialTimeFlag)
            serialTime++;

        // 按键
        if (keyBox[2])
            keyBox[2]++;
        if (keyBox[2] == 100) // 1s 后检查
        {
            key_action();
            keyBox[2] = 0; // 关闭自增
        }

        // pass
        
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
