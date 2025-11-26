#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"

/* Exported Variables --------------------------------------------------------*/

extern volatile BYTE serialSignBuf[];
extern volatile BYTE serialSignOk;
extern volatile BYTE serialPacketBuf[];
extern volatile BYTE serialPacketOk;
extern volatile BYTE serialPIDBuf[];
extern volatile BYTE serialPIDOk;
extern volatile BYTE serialCMDBuf[];
extern volatile BYTE serialCMDOk;

/* Exported Functions ------------------------------------------------------- */

// 初始化函数

void serial_init_3(UINT BaudRate); // 视觉串口
void serial_init_2(UINT BaudRate); // 调试串口
void serial_init_1(UINT BaudRate); // 调试串口

// 发送函数

void serial_send_byte(USART_TypeDef *USARTx, BYTE Byte);
void serial_send_string(USART_TypeDef *USARTx, BYTE *String);
void serial_printf(USART_TypeDef *USARTx, BYTE *format, ...);

// 处理函数

void serial_process_sign(void);
void serial_process_packet(void);
void serial_process_pid(void);
void serial_process_cmd(void);

// 等待函数

BYTE serial_wait_if(BYTE *flagString, BYTE (*getFlagFun)(void));
void serial_wait_while(BYTE *flagString, BYTE (*getFlagFun)(void));

// 获取标志位函数

BYTE get_sign_x_flag(void);
BYTE get_sign_d_flag(void);

#endif
