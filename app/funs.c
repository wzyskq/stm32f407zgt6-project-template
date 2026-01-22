#include "funs.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 阻塞延时 ******************** */

/******************************************************************
 * \brief      微秒级延时
 * \param[in]  us 延时时长，范围：0..25,565,281
 * \note       阻塞式延时，基于 SysTick 定时器实现
 */
void delay_us(u32 us)
{
    SysTick->LOAD = 168 * us;              // 设置定时器重装值
    SysTick->VAL  = 0x00;                  // 清空当前计数值
    SysTick->CTRL = 0x00000005;            // 设置时钟源为HCLK，启动定时器
    while (!(SysTick->CTRL & 0x00010000)); // 等待计数到0
    SysTick->CTRL = 0x00000004;            // 关闭定时器
}

/******************************************************************
 * \brief      毫秒级延时
 * \param[in]  ms 延时时长，范围：0..4,294,967,295
 * \note       阻塞式延时，基于 SysTick 定时器实现
 */
void delay_ms(u32 ms)
{
    while (ms--)
        delay_us(1000);
}

/******************************************************************
 * \brief      秒级延时
 * \param[in]  s 延时时长，范围：0..4,294,967,295
 * \note       阻塞式延时，基于 SysTick 定时器实现
 */
void delay_s(u32 s)
{
    while (s--)
        delay_ms(1000);
}

/* ******************** 阻塞延时 */

/*





*/

/* 字符串匹配 ******************** */

/******************************************************************
 * \brief      长字符串模式匹配函数
 * \param[in]  src 源字符串
 * \param[in]  pattern 模式字符串
 * \return     如果匹配成功，返回模式字符串后的位置指针；否则返回 NULL
 * \note       - 忽略 src 前导空格，匹配 pattern 全部内容
 *             - 该函数适用于长度大于 10Bytes 的模式匹配
 */
u8 *strmatch_l(u8 *src, u8 *pattern)
{
    while (*src == ' ') // 跳过前导空格
        src++;
    size_t len = strlen(pattern);
    if (!strncmp(src, pattern, len)) return src + len;
    return NULL;
}

/******************************************************************
 * \brief      短字符串模式匹配函数
 * \param[in]  src 源字符串
 * \param[in]  pattern 模式字符串
 * \return     如果匹配成功，返回模式字符串后的位置指针；否则返回 NULL
 * \note       - 忽略 src 前导空格，匹配 pattern 全部内容
 *             - 该函数适用于长度小于等于 10Bytes 的模式匹配
 */
u8 *strmatch_s(u8 *src, u8 *pattern)
{
    while (*src == ' ') // 跳过前导空格
        src++;
    while (*pattern != '\0')
        if (*src++ != *pattern++) return NULL;
    return src;
}

/* ******************** 字符串匹配 */

/*





*/

/******************************************************************
 * \brief  获取两个浮点数中的较小值
 */
float minf(float a, float b)
{
    return (a < b) ? a : b;
}

/******************************************************************
 * \brief  获取浮点数的绝对值
 */
s16 abs16(s16 num)
{
    return (num < 0) ? -num : num;
}
