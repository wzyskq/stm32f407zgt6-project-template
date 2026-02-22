#ifndef __IRQ_H
#define __IRQ_H

#include "main.h"
#include "sdio_sd.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

// TIM 中断

void TIM7_IRQHandler(void);

// USART 中断

void USART1_IRQHandler(void);
void USART3_IRQHandler(void);

#endif
