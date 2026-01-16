#ifndef __MAIN_H
#define __MAIN_H

/* System Includes --------------------------------------------------------- */

#include "stm32f4xx.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Exported Macro ---------------------------------------------------------- */

/* Exported Types ---------------------------------------------------------- */

typedef enum {
    normal = 0,
    inverse,
} sign_t;

/* User Includes ----------------------------------------------------------- */

#include "gpio.h"
#include "grayscale.h"
#include "i2c.h"
#include "key.h"
#include "mpu6050.h"
#include "oled.h"
#include "serial.h"
#include "timer.h"
#include "wheel.h"

#include "sys.h"
#include "funs.h"
#include "pid.h"

/* Exported Variables ------------------------------------------------------ */

/* Exported Functions ------------------------------------------------------ */

#endif /* __MAIN_H */
