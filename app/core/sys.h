#ifndef __SYS_H
#define __SYS_H

/* Exported Macro ---------------------------------------------------------- */

/* Exported Types ---------------------------------------------------------- */

typedef enum {
    trkObj_NULL = 0,
    trkObj_pos = 1,
    trkObj_dir = 2
} trkMode_t;

/* User Includes ----------------------------------------------------------- */

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

extern u8 oledViewIdx;
extern u32 sysTime;

// 巡线

extern u8 trkMode;
extern s16 trkSpd;

// 方向环

extern s16 carDeg;

// 位置环

extern u8 gray[];
extern s8 grayVal;

// 速度环 

extern s16 whlSpd[];
extern s16 whlCnt[];
extern s16 whlPwm[];

extern u8 spdStepDiv;
extern u8 spdStepErr;
extern u8 spdStepExp;

// 临时调试变量

extern u8 tWhlS;
extern s16 tWhlVn;

/* Exported Functions ------------------------------------------------------ */

void loop(void);
void speed_loop(void);
void position_loop(void);
void direction_loop(void);
void oled_ui(void);

#endif
