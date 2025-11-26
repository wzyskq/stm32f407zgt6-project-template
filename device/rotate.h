#ifndef __ROTATE_H_
#define __ROTATE_H_

#include "main.h"

// 初始化旋转电机控制 GPIO

void rotate_init(void); 

// 整体控制

void rotate_stop(void);
void rotate_forward(void);
void rotate_backword(void);

// 左轮控制

void rotate_stop_lt(void);
void rotate_forward_lt(void);
void rotate_backword_lt(void);

// 右轮控制

void rotate_stop_rt(void);
void rotate_forward_rt(void);
void rotate_backword_rt(void);

#endif
