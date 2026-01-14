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

/* 速度环 */ 

extern s16 whlSpd[];
extern s16 whlCnt[];
extern s16 whlPwm[];

extern u8 whlStepDiv;
extern u8 whlStepErr;
extern u8 whlStepExp;

/* 临时调试变量 */ 

extern u8 tWhlS;
extern s16 tWhlVn;

/* Exported Functions ------------------------------------------------------ */

void loop(void);
void speed_loop(void);
void oled_ui(void);

#endif
