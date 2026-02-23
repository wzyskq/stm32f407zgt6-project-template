#ifndef __SD_H
#define __SD_H

#include "ff.h"
#include "main.h"

/* Global Macros ----------------------------------------------------------- */

#define SDC_RESRL 1       // 调试返回串口号
#define SDC_PBUF_LEN 128  // 文件路径缓存大小
#define SDC_RBUF_LEN 2048 // 文件只读缓存大小

/* Global Types ------------------------------------------------------------ */

typedef enum
{
    sdc_rLine = 0,
    sdc_rFile
} sdcReadMode_t;

typedef enum
{
    KB = 0,
    MB,
    GB
} sdcStgUnit_t;

/* Global Variables -------------------------------------------------------- */

extern u8 sdcReFlag;
extern u8 sdcBuf[SDC_RBUF_LEN];
extern FRESULT mounted;

/* Global Functions -------------------------------------------------------- */

// 功能函数

u8 f_isopen(FIL *file);

// 基础函数

void sdc_init(void);
FRESULT sdc_get(u32 *allCapacity, u32 *freeCapacity, sdcStgUnit_t unit);

// 操作函数

void sdc_open(u8 *path);
void sdc_mkdir(u8 *path);
void sdc_close(void);
void sdc_delete(u8 *path);

// 读写函数

bool sdc_read(sdcReadMode_t mode, u32 line_num);
bool sdc_write(u8 *format, ...);

// void sdc_traversal(u8 *path);
// void sdc_log(u8 *format, ...);

#endif
