#ifndef __MAIN_H
#define __MAIN_H

/* System Includes -----------------------------------------------------------*/

#include "stm32f4xx.h"
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/* Exported Macro ------------------------------------------------------------*/

#define SERIAL_TIMEOUT 200    // 超时限制（单位：10ms）
#define SERIAL_BUF_SIZE 8     // 小缓冲区大小
#define SERIAL_BUFFER_SIZE 32 // 大缓冲区大小

/* Exported Types ------------------------------------------------------------*/

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t UINT;

/* User Includes -------------------------------------------------------------*/

#include "delay.h"
#include "funs.h"

#include "gpio.h"
#include "key.h"
#include "oled.h"
#include "pid.h"
#include "pwm.h"
#include "serial.h"
#include "timer.h"

/* Exported Variables --------------------------------------------------------*/

extern BYTE serialTimeFlag;
extern WORD serialTime;

extern WORD keyBox[];
extern BYTE keyFlag;

extern WORD pidErr;

/* Exported Functions ------------------------------------------------------- */

#endif /* __MAIN_H */
