#ifndef __KEY_H_
#define __KEY_H_

#include "main.h"

/* Exported Variables ------------------------------------------------------ */

extern u16 keyBox[];
extern u8 taskNum;

/* Exported Functions ------------------------------------------------------ */

void key_init(u8 keyNum);
void keys_init(void);

u8 key_scan(void);
void key_judge(void);
void key_action(void);

#endif
