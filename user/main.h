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

#include "delay.h"
#include "funs.h"

#include "gpio.h"
#include "key.h"
#include "oled.h"
#include "pid.h"
// #include "pwm.h"
#include "serial.h"
#include "timer.h"
#include "wheel.h"

/* Exported Variables ------------------------------------------------------ */

extern s16 pwm[];
extern s16 spd[];

/* Exported Functions ------------------------------------------------------ */

#endif /* __MAIN_H */
