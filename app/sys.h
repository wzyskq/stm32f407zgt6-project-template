#ifndef __SYS_H
#define __SYS_H

/* Exported Macro ---------------------------------------------------------- */

/* Exported Types ---------------------------------------------------------- */

/* User Includes ----------------------------------------------------------- */

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

extern u32 sysTime;
extern s16 pwm[];
extern s16 spd[];
extern s16 whlCnt[];

/* Exported Functions ------------------------------------------------------ */

void loop(void);

#endif
