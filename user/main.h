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

#define SERIAL_TIMEOUT 200 // 超时限制（单位：10ms）

/* Exported Types ---------------------------------------------------------- */

/* User Includes ----------------------------------------------------------- */

#include "delay.h"
#include "funs.h"

#include "gpio.h"
#include "key.h"
#include "oled.h"
#include "pid.h"
#include "rotate.h"
#include "pwm.h"
#include "serial.h"
#include "timer.h"

/* Exported Variables ------------------------------------------------------ */

extern u8 serialTimeFlag;
extern u16 serialTime;

extern u16 keyBox[];
extern u8 taskNum;

/* Exported Functions ------------------------------------------------------ */

#endif /* __MAIN_H */
