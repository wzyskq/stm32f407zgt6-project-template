#ifndef __SYS_H
#define __SYS_H

/* Exported Macro ---------------------------------------------------------- */

/* Exported Types ---------------------------------------------------------- */

/* User Includes ----------------------------------------------------------- */

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

extern u8 oledViewIdx;
extern u32 sysTime;
extern u8 whlTime;

extern u8 whlS;
extern s16 whlVn;

extern s16 whlSpd[];
extern s16 whlCnt[];
extern s16 whlPwm[];

// 速度环
extern u8 whlStepNum;
extern u8 whlMinStepErr;
extern u8 whlExponent;

/* Exported Functions ------------------------------------------------------ */

void loop(void);
void speed_loop(void);
void oled_ui(void);

#endif
