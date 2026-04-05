#include "key.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

static const key_s keyList[] = {
    [key0] = {RCC_AHB1Periph_GPIOA, GPIOA, GPIO_Pin_15, low, 2},  // PA15
    [key1] = {RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_0, low, 100}, // PC0
    [key2] = {RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_1, low, 100}, // PC1
    [key3] = {RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_2, low, 100}, // PC2
    [key4] = {RCC_AHB1Periph_GPIOC, GPIOC, GPIO_Pin_3, low, 100}, // PC3
    [key5] = {RCC_AHB1Periph_GPIOF, GPIOF, GPIO_Pin_5, low, 100}, // PF5
    [key6] = {RCC_AHB1Periph_GPIOF, GPIOF, GPIO_Pin_6, low, 100}, // PF6
};

/* Global Variables -------------------------------------------------------- */

keySts_s keySts = {0, 0, 0, 0}; // 按键时间 {按键编号, 按键次数, 按键时间}（单位：定时器中断周期）
u8 taskNum      = 0;            // 当前任务

/* Global Functions -------------------------------------------------------- */

/* 初始化函数 ******************** */

/******************************************************************
 * \brief      初始化指定按键
 * \param[in]  idx 按键编号
 * \note       请确保私有量 keyList 正确配置及 keyNum 置于配置索引末尾
 */
void key_init(key_e idx)
{
    RCC_AHB1PeriphClockCmd(keyList[idx].rccGpio, ENABLE);

    GPIO_InitTypeDef gpio = {0};
    gpio.GPIO_Pin   = keyList[idx].pin;                                        // 指定按键
    gpio.GPIO_Speed = GPIO_Speed_50MHz;                                        // 50MHz
    gpio.GPIO_Mode  = GPIO_Mode_IN;                                            // 普通输入模式
    gpio.GPIO_PuPd  = keyList[idx].keyOnLevel ? GPIO_PuPd_DOWN : GPIO_PuPd_UP; // 根据按下电平配置上拉或下拉
    GPIO_Init(keyList[idx].gpio, &gpio);
}

/******************************************************************
 * \brief  初始化所有按键
 * \note   请先确保 keyNum 置于配置索引末尾
 */
void keys_init(void)
{
    for (u8 i = 0; i < keyNum; i++)
        key_init((key_e)i);
}

/* ******************** 初始化函数 */

/*





*/

/* 功能函数 ******************** */

/******************************************************************
 * \brief      读取指定按键状态
 * \param[in]  idx 按键编号
 * \retval     bool 按键状态
 */
bool key_read(key_e idx)
{
    if (GPIO_ReadInputDataBit(keyList[idx].gpio, keyList[idx].pin) == keyList[idx].keyOnLevel) {
        delay_ms(20);
        while (GPIO_ReadInputDataBit(keyList[idx].gpio, keyList[idx].pin) == keyList[idx].keyOnLevel);
        return true;
    }
    return false;
}

/******************************************************************
 * \brief   扫描按键
 * \return  按键索引，keyNum 表示无按键按下
 * \note    请先确保 keyNum 置于配置索引末尾
 */
key_e key_scan(void)
{
    for (u8 i = 0; i < keyNum; i++) {
        if (key_read((key_e)i))
            return (key_e)i;
    }
    return keyNum;
}

/******************************************************************
 * @brief  按键检测
 */
void key_judge(void)
{
    key_e keyst = key_scan();
    if (keyst != keyNum) {
        if (keyst != keySts.idx || keySts.tim == 0) // 重置按键条件：按键不同 或 按键计时为零
        {
            keySts.idx      = keyst;
            keySts.cnt      = 1;
            keySts.tim      = 1; // 置1，开启在定时中断中自增
            keySts.overtime = keyList[keyst].overtime;
        } else {
            keySts.cnt++;
        }

        if (keySts.idx) {
            oled_printf(0, 48, OLED_8X16, "%d_%d", keySts.idx, keySts.cnt);
            // oled_update();
        }
    }
}

/******************************************************************
 * @brief  执行按键动作
 */
void key_action(void)
{

    if (keySts.idx == key0) {
        if (keySts.cnt == 1)
            led_turn(led0);
    } else if (keySts.idx == key1) {
        if (keySts.cnt == 1)
            taskNum = 1;
        // else if (keySts.cnt == 2)
        //     taskNum = 5;
    } else if (keySts.idx == key2) {
        if (keySts.cnt == 1)
            taskNum = 2;
        // else if (keySts.cnt == 2)
        //     taskNum = 6;
    } else if (keySts.idx == key3) {
        if (keySts.cnt == 1)
            taskNum = 3;
        // else if (keySts.cnt == 2)
        //     taskNum = 7;
    } else if (keySts.idx == key4) {
        if (keySts.cnt == 1)
            taskNum = 4;
        // else if (keySts.cnt == 2)
        //     taskNum = 8;
    } else if (keySts.idx == key5) {
        if (keySts.cnt == 1)
            taskNum = 5;
        // else if (keySts.cnt == 2)
        // taskNum = 10;
    } else if (keySts.idx == key6) {
        if (keySts.cnt == 1)
            taskNum = 6;
        // else if (keySts.cnt == 2)
        // taskNum = 12;
    }

    oled_printf(0, 48, OLED_8X16, "%d-%d", keySts.idx, keySts.cnt);
    // oled_update();

    if (keySts.idx)
        keySts.idx = 0;
}

/* ******************** 功能函数 */
