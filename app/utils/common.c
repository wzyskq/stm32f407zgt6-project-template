#include "common.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

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

/* 字符串处理 ******************** */

/******************************************************************
 * \brief       字符串匹配函数
 * \param[in]   pattern 模型字符串指针
 * \param[in]   srcPtr 源字符串指针
 * \param[out]  endPtr 如果匹配成功，返回模式字符串后的位置指针地址
 * \retval      bool 是否匹配成功
 * \note        - 忽略 srcPtr 前导空格，匹配 pattern 全部内容
 *              - 对于长于 10 字符的模型字符串，性能更优
 */
bool strmatch(u8 *pattern, u8 *srcPtr, u8 **endPtr)
{
    while (*srcPtr == ' ') // 跳过前导空格
        srcPtr++;
    size_t len = strlen(pattern);
    if (!strncmp(srcPtr, pattern, len))
    {
        if (endPtr)
            *endPtr = srcPtr + len; // 设置 endPtr 指向匹配后的位置
        return true;
    }
    return false;
}

/******************************************************************
 * \brief       字符串匹配函数（严格匹配）
 * \param[in]   pattern 模型字符串指针
 * \param[in]   srcPtr 源字符串指针
 * \param[out]  endPtr 如果匹配成功，返回模式字符串后的位置指针地址
 * \retval      bool 是否匹配成功
 * \note        - 忽略 srcPtr 前导空格，严格匹配 pattern 全部内容
 *              - pattern 后必须是空格或字符串结束符
 *              - 对于短于 10 字符的模式字符串，性能更优
 */
bool strmatch_s(u8 *pattern, u8 *srcPtr, u8 **endPtr)
{
    while (*srcPtr == ' ') // 跳过前导空格
        srcPtr++;
    while (*pattern != '\0')
        if (*srcPtr++ != *pattern++)
            return false;
    if (endPtr)
        *endPtr = srcPtr;
    return true;
}

/******************************************************************
 * \brief       字符串转十六进制函数
 * \param[in]   str 输入字符串指针
 * \param[out]  hex 输出十六进制字符串指针
 * \note        - 字符串末尾必须以 '\0' 结尾
 *              - hex 必须有足够的空间存储转换后的十六进制字符串
 */
void strtohex(u8 *str, u8 *hex)
{
    for (u16 i = 0; str[i] != '\0'; i++)
        sprintf(hex + i * 3, "%02X ", str[i]);
}

/* ******************** 字符串处理 */

/*





*/

/* 数值处理 ******************** */

/******************************************************************
 * \brief  获取浮点数的绝对值
 */
s16 abs16(s16 num)
{
    return (num < 0) ? -num : num;
}

/* ******************** 数值处理 */
