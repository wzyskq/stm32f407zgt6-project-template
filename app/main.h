#ifndef __MAIN_H
#define __MAIN_H

/* System Includes --------------------------------------------------------- */

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

#define ABS(x)    ((x) > 0 ? (x) : -(x))  // 伪泛型绝对值
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // 伪泛型最小值
#define MAX(a, b) ((a) > (b) ? (a) : (b)) // 伪泛型最大值

// 优先级重映射

#define PreemptingPriority(x) ((x) < 10 ? 0 : (x) / 10)
#define SubPriority(x)        ((x) % 10)

/* Private Types ----------------------------------------------------------- */

/* User Includes ------------------------------------------------------------ */

// 业务及工具

#include "irq.h"
#include "sys.h"
#include "task.h"
#include "common.h"
#include "misc.h"
#include "pid.h"
#include "types.h"

// 常用外设

#include "grayscale.h"
#include "key.h"
#include "led.h"
#include "wheel.h"
#include "i2c.h"
#include "mpu6050.h"
#include "oled.h"
#include "sdc.h"
#include "timer.h"
#include "serial.h"

// 三方组件

#include "zdt_api.h"
#include "diskio.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

#endif /* __MAIN_H */
