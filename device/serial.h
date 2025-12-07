#ifndef __SERIAL_H
#define __SERIAL_H

#include "main.h"

/* Exported Macro ---------------------------------------------------------- */

#define SRL_SIGBUF_LEN 8
#define SRL_PKGBUF_LEN 8
#define SRL_PIDBUF_LEN 16
#define SRL_CMDBUF_LEN 1024

#define SRL_PRINTF_LEN 1024

/* Exported Variables ------------------------------------------------------ */

extern u8 srlReFlag; // 串口调试返回标志位

extern volatile u8 srlSigBuf[];
extern volatile u8 srlSigFlg;
extern volatile u8 srlPidBuf[];
extern volatile u8 srlPidFlg;
extern volatile u8 srlCmdBuf[];
extern volatile u8 srlCmdFlg;
extern volatile u8 srlPkgBuf[];
extern volatile u8 srlPkgFlg;

/* Exported Functions ------------------------------------------------------ */

// 初始化函数

void serial_init(u8 srlNum, u32 baudRate, u8 subPriority);

// 发送函数

void serial_send_byte(USART_TypeDef *USARTx, u8 Byte);
void serial_send_string(USART_TypeDef *USARTx, u8 *String);
void serial_printf(USART_TypeDef *USARTx, const char *format, ...);

// 处理函数

void serial_decode_sign(void);
void serial_decode_packet(void);
void serial_decode_pid(void);
void serial_decode_cmd(void);

// 等待函数

u8 serial_wait_if(u8 *flagString, u8 (*getFlagFun)(void));
void serial_wait_while(u8 *flagString, u8 (*getFlagFun)(void));

#endif
