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

/* Global Types ------------------------------------------------------------ */

typedef enum {
    normal = 0,
    inverse,
} sign_t;

/* User Includes ------------------------------------------------------------ */

#include "funs.h"
#include "pid.h"
#include "irq.h"
#include "task.h"
#include "sys.h"

#include "grayscale.h"
#include "key.h"
#include "wheel.h"
#include "i2c.h"
#include "mpu6050.h"
#include "oled.h"
#include "timer.h"
#include "serial.h"

#include "zdt_api.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

#endif /* __MAIN_H */
