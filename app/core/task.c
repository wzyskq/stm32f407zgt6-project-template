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
    _s32 t = Emm_V5_Get_Sys_Params();
    if (!t.isNull) {
        serial_printf(1, "V> %d\n", t.v);
        zdtTvFlg = false;
        taskNum  = 102;
    } else
        return;
}

void await_2(void)
{
    _s32 t = Emm_V5_Get_Sys_Params();
    if (!t.isNull) {
        serial_printf(1, "P> %d\n", t.v);
        zdtTvFlg = false;
        taskNum  = 101;
    } else
        return;
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
