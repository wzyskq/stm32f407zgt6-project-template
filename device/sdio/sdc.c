#include "sdc.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

FIL fdst;
FATFS fs;

u8 prePath[SDC_PBUF_LEN] = {0}; // 上次打开的文件路径
u32 br, bw;
u32 curLine = 0; // 当前行计数器

/* Global Variables -------------------------------------------------------- */

u8 sdcReFlag            = 1;   // 调试返回模式：1 开启，0 关闭
u8 sdcBuf[SDC_RBUF_LEN] = {0}; // SD卡只读缓存
FRESULT mounted;               // 挂载结果

/* Global Functions -------------------------------------------------------- */

/* 功能函数 ******************** */

/******************************************************************
 * \brief      检查文件对象是否已经打开
 * \param[in]  file: 文件对象指针
 * \return     1: 已打开, 0: 未打开
 */
u8 f_isopen(FIL *file)
{
    if (file != NULL && file->obj.fs != NULL)
        return 1; // 文件已打开
    return 0;     // 文件未打开
}

/* ******************** 功能函数 */

/*





*/

/* 基础函数 ******************** */

/******************************************************************
 * \brief  初始化SD卡
 * \note   返回挂载结果到全局变量 mounted . 
 */
void sdc_init(void)
{
    // 配置中断优先级
    NVIC_InitTypeDef nvic = {0};
    nvic.NVIC_IRQChannel                   = SDIO_IRQn; // SDIO
    nvic.NVIC_IRQChannelCmd                = ENABLE;    // 使能SDIO中断
    nvic.NVIC_IRQChannelPreemptionPriority = 0;         // SDIO中断抢占优先级最高
    nvic.NVIC_IRQChannelSubPriority        = 0;         // SDIO中断优先级最高
    NVIC_Init(&nvic);

    // 尝试初始化 SD卡，最多5次
    DSTATUS s = 0;
    for (int i = 0; i < 5; ++i) {
        s = disk_initialize(0); // 初始化SD卡（设备号0）
        if (s == 0)             // 如果初始化成功
            break;
    }
    mounted = f_mount(&fs, "", 1); // 立即挂载SD卡
}

/******************************************************************
 * \brief       获取SD卡总容量和剩余容量
 * \param[out]  allCapacity   总容量
 * \param[out]  freeCapacity  剩余容量
 * \param[in]   unit          存储单位，KB/MB/GB
 * \retval      FRESULT       操作结果
 */
FRESULT sdc_get(u32 *allCapacity, u32 *freeCapacity, sdcStgUnit_t unit)
{
    FRESULT fr = FR_OK;
    DWORD freeClust, freeSect, totalSect;
    FATFS *pfs;
    u32 divisor = 2;

    // 根据单位确定除数
    if (unit == KB)
        divisor = 2; // 1KB = 2 个 512 字节扇区
    else if (unit == MB)
        divisor = 2048; // 1MB = 2048 个 512 字节扇区
    else if (unit == GB)
        divisor = 2097152; // 1GB = 2097152 个 512 字节扇区
    fr = f_getfree("", &freeClust, &pfs);

    if (fr == FR_OK) {
        totalSect = (pfs->n_fatent - 2) * pfs->csize;
        freeSect  = freeClust * pfs->csize;
        // 计算总容量和剩余容量
        if (allCapacity)
            *allCapacity = totalSect / divisor;
        if (freeCapacity)
            *freeCapacity = freeSect / divisor;
    } else {
        // 获取失败时清零输出参数
        if (allCapacity)
            *allCapacity = 0;
        if (freeCapacity)
            *freeCapacity = 0;
        if (sdcReFlag)
            serial_printf(SDC_RESRL, "Failed to get SD card free space. Error: (%d)\r\n", fr);
    }
    return fr;
}

/* ******************** 基础函数 */

/*





*/

/* 操作函数 ******************** */

/******************************************************************
 * \brief      打开SD卡文件
 * \param[in]  path: 文件路径
 *
 * \note  1. 防重开机制逻辑：
 *             对比上次打开的文件路径与当前路径
 *              ├──不相同，关闭上次文件，打开新文件
 *              └──相同，检查文件是否已打开
 *                  ├──已打开，不再重复打开
 *                  └──未打开，打开新文件
 *        2. 打开文件时使用 FA_OPEN_APPEND | FA_WRITE | FA_READ 模式
 *        3. 打开失败时最多尝试5次
 */
void sdc_open(u8 *path)
{
    FRESULT fr  = FR_OK;
    u8 attempts = 0;

    if (strcmp(prePath, path) != 0) // 路径不相同
    {
        sdc_close();
    } else // 路径相同
    {
        if (f_isopen(&fdst)) {
            if (sdcReFlag)
                serial_printf(SDC_RESRL, "File (%s) is already open.\r\n", path);
            return; // 文件已打开，直接返回
        }
    }

    // 尝试打开文件，最多5次
    while (attempts < 5) {
        fr = f_open(&fdst, path, FA_OPEN_APPEND | FA_WRITE | FA_READ);
        if (fr == FR_OK) {
            snprintf(prePath, SDC_PBUF_LEN - 1, "%s", path); // 更新历史路径

            if (sdcReFlag)
                serial_printf(SDC_RESRL, "File (%s) opened successfully.\r\n", path);
            f_lseek(&fdst, 0); // 重置文件指针到开头
            curLine = 0;       // 重置行计数器
            return;            // 打开成功，返回
        }
        ++attempts;
    }

    if (sdcReFlag)
        serial_printf(SDC_RESRL, "Failed to open file (%s). Error: (%d)\r\n", path, fr);
    return; // 打开失败，返回
}

/******************************************************************
 * \brief  关闭SD卡文件
 *
 * \note  如果文件已打开，则关闭文件并打印成功信息
 */
void sdc_close(void)
{
    if (f_isopen(&fdst)) {
        f_close(&fdst);

        if (sdcReFlag)
            serial_printf(SDC_RESRL, "File (%s) closed successfully.\r\n", prePath);
    }
}

/******************************************************************
 * \brief      删除SD卡文件/文件夹
 * \param[in]  path: 文件路径
 *
 * \note  拒绝删除原因：
 *          1. 目标对象不能为只读文件
 *          2. 目标文件夹必须为空
 *          3. 目录对象不能被打开（函数已内置）
 */
void sdc_delete(u8 *path)
{
    FRESULT fr = FR_OK;

    sdc_close(); // 确保文件已关闭
    fr = f_unlink(path);
    if (fr == FR_OK) {
        if (sdcReFlag)
            serial_printf(SDC_RESRL, "File (%s) deleted successfully.\r\n", path);
        return; // 删除成功，返回
    }

    if (sdcReFlag)
        serial_printf(SDC_RESRL, "Failed to delete (%s). Error: (%d)\r\n", path, fr);
    return; // 删除失败，返回
}

/******************************************************************
 * \brief      创建SD卡文件夹
 * \param[in]  path: 目录路径
 *
 * \note  1. 创建前先检查目录是否已存在
 *        2. 如果目录已存在，则不执行任何操作
 *        3. 如果目录创建失败，则重复尝试，最多5次，超过则打印错误信息
 */
void sdc_mkdir(u8 *path)
{
    FRESULT fr  = FR_OK;
    u8 attempts = 0;

    // 检查目录是否已存在
    if (f_stat(path, NULL) == FR_OK) {
        if (sdcReFlag)
            serial_printf(SDC_RESRL, "Directory (%s) already exists.\r\n", path);
        return;
    }
    // 尝试创建目录，最多5次
    while (attempts < 5) {
        fr = f_mkdir(path); // 创建目录
        if (fr == FR_OK) {
            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Directory (%s) created successfully.\r\n", path);
            return; // 创建成功，返回
        }
        ++attempts;
    }

    if (sdcReFlag)
        serial_printf(SDC_RESRL, "Failed to create directory (%s). Error: (%d)\r\n", path, fr);
    return; // 创建失败，返回
}

/* ******************** 操作函数 */

/*





*/

/* 读写函数 ******************** */

/******************************************************************
 * \brief      读取SD卡文件
 * \param[in]  mode: 读取模式
 *   \arg        sdc_rLine: 读取一行
 *   \arg        sdc_rFile: 读取全部
 * \param[in]  lineNum: 读取的行号（仅在 sdc_rLine 模式下有效）
 *   \arg        =0: 读取当前光标所在行
 *   \arg        >0: 读取指定行号
 * \retval     bool: 读取结果 
 *
 * \note  1. 没有长度超限保护（默认 2048 B），超限会导致缓冲区溢出系统，死机！！！
 *           建议使用行读取模式 READ_LINE，避免一次性读取过多数据（前提单行也在 2048 B以内）
 *        2. 不包含文件打开操作，需先调用 sdc_open 函数打开文件
 *        3. 读取的数据存储在: sdcBuf (全局变量)
 */
bool sdc_read(sdcReadMode_t mode, u32 lineNum)
{
    if (!f_isopen(&fdst)) {
        if (sdcReFlag)
            serial_printf(SDC_RESRL, "File (%s) is not open. Please open it first.\r\n", prePath);
        return false; // 文件未打开
    }

    if (mode == sdc_rLine) // 读取一行
    {
        TCHAR *frLine = NULL; // f_gets 返回值（即输入的缓冲区指针）
        // static u32 curLine = 0; // 改为全局变量，记录当前行号，以保证每次打开都清零行号

        if (lineNum == 0) {
            // 读取当前行
            frLine = f_gets((TCHAR *)sdcBuf, sizeof(sdcBuf), &fdst);
            if (frLine == NULL) {
                if (sdcReFlag)
                    serial_printf(SDC_RESRL, "Failed to read current line from file (%s).\r\n", prePath);
                return false; // 读取失败或到达文件末尾
            }
            ++curLine;

            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Read current line %u (%d B): \r\n%s\r\n", curLine, strlen(sdcBuf), sdcBuf);
            return true; // 读取成功
        } else {
            // 读取指定行号（lineNum 从 1 开始，1 为第一行）
            f_lseek(&fdst, 0); // 文件指针移到开头
            curLine = 0;
            while (curLine < lineNum) {
                frLine = f_gets((TCHAR *)sdcBuf, sizeof(sdcBuf), &fdst);
                if (frLine == NULL) {
                    if (sdcReFlag)
                        serial_printf(SDC_RESRL, "Failed to read line %u from file (%s).\r\n", lineNum, prePath);
                    return false; // 读取失败或到达文件末尾
                }
                ++curLine;
            }

            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Read line %u (%d B): \r\n%s\r\n", curLine, strlen(sdcBuf), sdcBuf);
            return true; // 读取成功
        }
    } else if (mode == sdc_rFile) // 读取全部
    {
        f_lseek(&fdst, 0);
        FRESULT fr = f_read(&fdst, sdcBuf, sizeof(sdcBuf) - 1, &br);
        if (fr == FR_OK && br > 0) {
            sdcBuf[br] = '\0'; // 手动添加字符串结束符，防止附带之前的数据
            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Read all data (%d B): \r\n%s\r\n", strlen(sdcBuf), sdcBuf);
            return true; // 读取成功
        } else {
            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Failed to read all data from file (%s). Error: (%d)\r\n", prePath, fr);
            return false; // 读取失败
        }
    }
}

/******************************************************************
 * \brief      写入SD卡文件（支持格式化输出）
 * \param[in]  format: 格式化字符串
 * \param[in]  ...: 可变参数
 *
 * \note  1. 没有格式化的字符串长度超限保护（默认 2048 B），超限会导致缓冲区溢出系统，死机！！！
 *           使用时确保格式化后的字符串长度不超过 2048 B
 *        2. 不包含文件打开操作，需先调用 sdc_open 函数打开文件
 *        3. 写入后会自动刷新文件，防止数据丢失
 */
bool sdc_write(u8 *format, ...)
{
    if (!f_isopen(&fdst)) {
        if (sdcReFlag)
            serial_printf(SDC_RESRL, "File (%s) is not open. Please open it first.\r\n", prePath);
        return false; // 文件未打开
    }

    FRESULT fr = FR_OK;
    u8 data[SDC_RBUF_LEN];
    va_list args;

    // 格式化数据到缓冲区
    va_start(args, format);
    vsprintf(data, format, args);
    va_end(args);

    fr = f_write(&fdst, data, strlen(data), &bw); // 写入数据
    if (fr == FR_OK && bw > 0) {
        if (!f_sync(&fdst)) {
            if (sdcReFlag)
                serial_printf(SDC_RESRL, "Data written successfully to file (%s).\r\n", prePath);
            return true; // 写入成功，返回
        }

        if (sdcReFlag)
            serial_printf(SDC_RESRL, "Failed to sync file (%s) after write. Error: (%d)\r\n", prePath, fr);
        return false; // 同步失败，返回
    }

    if (sdcReFlag)
        serial_printf(SDC_RESRL, "Failed to write data to file (%s). Error: (%d)\r\n", prePath, fr);
    return false; // 写入失败，返回
}

/* ******************** 读写函数 */

// /**
//  * @brief       遍历指定SD卡目录，打开未满文件
//  * @param[in]   path: 文件夹名，sample/overLimit/hideData
//  *
//  * @note  遍历目录下的所有 .txt 文件，查找或创建未满10行的文件，打开
//  */
// void sdc_traversal(u8 *path)
// {
//     FRESULT fr;
//     DIR dj;
//     FILINFO fno;
//     u8 filepath[SDC_PBUF_LEN];
//     u8 newfile[SDC_PBUF_LEN];
//     u8 timeStr[16] = {0};
//     u8 found = 0;
//
//     // 打开目录，遍历所有 .txt 文件
//     fr = f_findfirst(&dj, &fno, path, "*.txt");
//     while (fr == FR_OK && fno.fname[0])
//     {
//         // 拼接完整路径
//         sprintf(filepath, "%s/%s", path, fno.fname);
//
//         // 打开文件
//         sdc_open(filepath);
//
//         // 判断是否满10行
//         if (!sdc_read(sdc_rLine, 10))
//         {
//             // 找到未满10行的文件，直接返回
//             found = 1;
//
//             if (sdcReFlag)
//                 serial_printf(SDC_RESRL, "Found file (%s), open for write.\r\n", filepath);
//             f_closedir(&dj);
//             return;
//         }
//         // 关闭当前文件，继续找下一个
//         sdc_close();
//         fr = f_findnext(&dj, &fno);
//     }
//     f_closedir(&dj);
//
//     // 没有未满10行的文件或没有文件，创建新文件
//     if (!found)
//     {
//         // 获取当前时间字符串
//         snprintf(timeStr, sizeof(timeStr), "20%02d%02d%02d%02d%02d%02d",
//                  T.year, T.month, T.date, T.hour, T.min, T.sec);
//
//         // 生成新文件名
//         if (strcmp(path, "sample") == 0)
//             sprintf(newfile, "%s/sampleData%s.txt", path, timeStr);
//         else if (strcmp(path, "overLimit") == 0)
//             sprintf(newfile, "%s/overLimit%s.txt", path, timeStr);
//         else if (strcmp(path, "hideData") == 0)
//             sprintf(newfile, "%s/hideData%s.txt", path, timeStr);
//
//         sdc_open(newfile);
//
//         if (sdcReFlag)
//             serial_printf(SDC_RESRL, "All files full or no file, created and opened new file: %s\r\n", newfile);
//     }
// }
//
// /**
//  * @brief      记录日志（支持格式化输出）
//  * @param[in]  format: 格式化字符串
//  * @param[in]  ...: 可变参数
//  *
//  * @note  1. 没有格式化的字符串长度超限保护（默认 2048 B），超限会导致缓冲区溢出系统，死机！！！
//  *           使用时确保格式化后的字符串长度不超过 2048 B
//  *        2. 写入安全：包含独立文件打开操作，若文件夹不存在则会自动关闭当前文件（sdc_open()安全机制），后续也不会写入
//  *        3. 若成功写入，会自动刷新保存
//  *        4. 日志文件名格式为 log/logX.txt，其中 X 为复位次数
//  */
// void sdc_log(u8 *format, ...)
// {
//     u8 filepath[32];
//     u8 timeStr[24]; // 时间字符串缓冲区
//     u8 data[SDC_RBUF_LEN];
//     va_list args;
//
//     // 格式化时间
//     snprintf(timeStr, 24, "20%02d-%02d-%02d %02d:%02d:%02d",
//              T.year, T.month, T.date, T.hour, T.min, T.sec);
//
//     // 格式化日志数据
//     va_start(args, format);
//     vsprintf(data, format, args);
//     va_end(args);
//
//     sprintf(filepath, "log/log%d.txt", storeData[2]); // 清除记录后首次上电系统自增 1，若想要获得 0，需减 1，但是当时根本没有插卡（题目逻辑问题），所以就不减了
//     sdc_open(filepath);
//     if (!f_isopen(&fdst))
//     {
//         if (sdcReFlag)
//             serial_printf(SDC_RESRL, "Failed to open log file (%s).\r\n", filepath);
//         return; // 打开失败，返回
//     }
//
//     f_lseek(&fdst, f_size(&fdst)); // 移动到文件末尾
//     if (f_printf(&fdst, "%s %s\r\n", timeStr, data) >= 0)
//     {
//         if (!f_sync(&fdst))
//         {
//             if (sdcReFlag)
//                 serial_printf(SDC_RESRL, "Log data written successfully to file (%s).\r\n", filepath);
//             return; // 写入成功，返回
//         }
//
//         if (sdcReFlag)
//             serial_printf(SDC_RESRL, "Failed to sync log file (%s) after write.\r\n", filepath);
//         return; // 同步失败，返回
//     }
//
//     if (sdcReFlag)
//         serial_printf(SDC_RESRL, "Failed to write log data to file (%s).\r\n", filepath);
//     return; // 写入失败，返回
// }
