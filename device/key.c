#include "key.h"

/* Private Macros ---------------------------------------------------------- */

#define KEY_NUM 4 // 按键数量

/* Private Variables ------------------------------------------------------- */

static const u32 keyRccGpio[]      = {0, RCC_AHB1Periph_GPIOF, RCC_AHB1Periph_GPIOF, RCC_AHB1Periph_GPIOF, RCC_AHB1Periph_GPIOF};
static GPIO_TypeDef *keyGpioPort[] = {0, GPIOF, GPIOF, GPIOF, GPIOF};
static const u16 keyGpioPin[]      = {0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4};

/* Global Variables -------------------------------------------------------- */

// 按键
u16 keyBox[] = {0, 0, 0}; // 按键时间 {按键编号, 按键次数, 按键时间} 单位：10ms
u8 taskNum   = 0;         // 当前按键

/*





*/

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化指定按键
 * \param[in]  keyNum 按键编号，x=1..4
 * \note       强烈建议使用 keys_init() 初始化所有按键
 */
void key_init(u8 keyNum)
{
    if (keyNum < 1 || keyNum > KEY_NUM) return;

    RCC_AHB1PeriphClockCmd(keyRccGpio[keyNum], ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = keyGpioPin[keyNum]; // 指定按键
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 50MHz
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;       // 普通输入模式
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;       // 上拉
    GPIO_Init(keyGpioPort[keyNum], &GPIO_InitStructure);
}

/******************************************************************
 * \brief  初始化所有按键
 * \note   请先确保私有宏 KEY_NUM 已正确设置
 */
void keys_init(void)
{
    for (u8 i = 1; i <= KEY_NUM; i++)
        key_init(i);
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * @brief   扫描按键
 * @return  按键编号，0 表示无按键按下
 */
u8 key_scan(void)
{
    for (u8 i = 1; i <= KEY_NUM; i++) {
        if (GPIO_ReadInputDataBit(keyGpioPort[i], keyGpioPin[i]) == 0) {
            delay_ms(20);
            while (GPIO_ReadInputDataBit(keyGpioPort[i], keyGpioPin[i]) == 0);
            return i;
        }
    }

    return 0;
}

/******************************************************************
 * @brief  按键检测
 */
void key_judge(void)
{
    u8 keyst = key_scan();
    if (keyst) {
        if (keyst != keyBox[0] || keyBox[2] == 0) // 重置按键条件：按键不同 或 按键计时为零
        {
            for (u8 i = 0; i < 3; i++)
                keyBox[i] = 0;

            keyBox[0] = keyst;
            keyBox[1] = 1;
            keyBox[2] = 1; // 设置为 1，开启在定时中断中自增
        } else {
            keyBox[1]++;
        }

        if (keyBox[0]) {
            oled_printf(0, 48, OLED_8X16, "%d_%d", keyBox[0], keyBox[1]);
            // oled_update();
        }
    }
}

/******************************************************************
 * @brief  执行按键动作
 */
void key_action(void)
{

    if (keyBox[0] == 1) {
        if (keyBox[1] == 1)
            taskNum = 1;
        else if (keyBox[1] == 2)
            taskNum = 2;
    } else if (keyBox[0] == 2) {
        if (keyBox[1] == 1)
            taskNum = 3;
        else if (keyBox[1] == 2)
            taskNum = 4;
    } else if (keyBox[0] == 3) {
        if (keyBox[1] == 1)
            taskNum = 5;
        else if (keyBox[1] == 2)
            taskNum = 6;
    } else if (keyBox[0] == 4) {
        if (keyBox[1] == 1)
            taskNum = 7;
        else if (keyBox[1] == 2)
            taskNum = 8;
    }

    oled_printf(0, 48, OLED_8X16, "%d-%d", keyBox[0], keyBox[1]);
    // oled_update();

    if (keyBox[0])
        keyBox[0] = 0;
}

/* ******************** 功能函数 */
