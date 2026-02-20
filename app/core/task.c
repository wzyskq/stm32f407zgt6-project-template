#include "task.h"

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

void future_1(void)
{
    if (!zdtTvFlg) {
        serial_printf(1, "Start Read S_VEL\n");
        Emm_V5_Read_Sys_Params(4, 1, S_VEL);
    }
}

void future_2(void)
{
    if (!zdtTvFlg) {
        serial_printf(1, "Start Read S_TPOS\n");
        Emm_V5_Read_Sys_Params(4, 1, S_TPOS);
    }
}

void await_1(void)
{
    Emm_V5_Get_Sys_Params(&zdtSysData);
    while(zdtRvFlg); // 等待清零
    serial_printf(1, "V> %d\n", (s32)zdtSysData.vel);
    taskNum = 102;
}

void await_2(void)
{
    Emm_V5_Get_Sys_Params(&zdtSysData);
    while(zdtRvFlg); // 等待清零
    serial_printf(1, "P> %d\n", (s32)zdtSysData.tpos);
    taskNum = 101;
}

void demo_1(void)
{
    await_1();
    future_1();
}

void demo_2(void)
{
    await_2();
    future_2();
}
