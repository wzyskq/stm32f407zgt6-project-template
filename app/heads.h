#ifndef __HEADS_H
#define __HEADS_H

/* Global Includes --------------------------------------------------------- */

#include "stm32f4xx.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

/* Global Macro ------------------------------------------------------------ */

#define FRQ 84

// 常用工具宏

#define ABS(x)    ((x) < 0 ? -(x) : (x))  // 伪泛型绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // 伪泛型最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // 伪泛型最大值
#define SIGN(x)   ((x) >= 0 ? 1 : -1)     // 伪泛型符号值

// 优先级重映射

#define PreemptingPriority(x) ((x) < 10 ? 0 : (x) / 10)
#define SubPriority(x)        ((x) % 10)

/* Global Types ------------------------------------------------------------ */

// 电平类型
typedef enum {
    low  = 0,
    high = !low,
} level_e;

// 方向类型
typedef enum {
    normal = 0,
    inverse = !normal,
} sign_e;

#endif
