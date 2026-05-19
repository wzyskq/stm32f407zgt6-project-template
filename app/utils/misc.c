#include "misc.h"

/* Private Macros ---------------------------------------------------------- */

/* Private Variables ------------------------------------------------------- */

/* Global Variables -------------------------------------------------------- */

/* Global Functions -------------------------------------------------------- */

/* 执行函数 ******************** */

/******************************************************************
 * @brief  执行按键动作
 */
void key_action(void)
{

    if (keySts.idx == key0) {
        if (keySts.cnt == 1)
            led_turn(led0);
    } else if (keySts.idx == key1) {
        if (keySts.cnt == 1)
            taskNum = 1;
        // else if (keySts.cnt == 2)
        //     taskNum = 5;
    } else if (keySts.idx == key2) {
        if (keySts.cnt == 1)
            taskNum = 2;
        // else if (keySts.cnt == 2)
        //     taskNum = 6;
    } else if (keySts.idx == key3) {
        if (keySts.cnt == 1)
            taskNum = 3;
        // else if (keySts.cnt == 2)
        //     taskNum = 7;
    } else if (keySts.idx == key4) {
        if (keySts.cnt == 1)
            taskNum = 4;
        // else if (keySts.cnt == 2)
        //     taskNum = 8;
    } else if (keySts.idx == key5) {
        if (keySts.cnt == 1)
            taskNum = 5;
        // else if (keySts.cnt == 2)
        // taskNum = 10;
    } else if (keySts.idx == key6) {
        if (keySts.cnt == 1)
            taskNum = 6;
        // else if (keySts.cnt == 2)
        // taskNum = 12;
    }

    oled_printf(0, 48, OLED_8X16, "%d-%d", keySts.idx, keySts.cnt);
    // oled_update();

    if (keySts.idx)
        keySts.idx = 0;
}

/* ******************** 执行函数 */

/*





*/

/* 串口解析函数 ******************** */

/******************************************************************
 * \brief  解析 srlPidBuf 数据包内容
 * \note   在主循环中调用，当 srlPidFlg 为 1 时
 */
void misc_srlParse_pid(void)
{
    if (!srlPidFlg)
        return;

    u8 len = srlPidBuf[0];
    if (len < 4) { // 至少要有一个字节
        srlPidFlg = 0;
        return;
    }

    // // 数据实际从 srlPidBuf[1] 开始
    // char type = srlPidBuf[1];
    // u8 num    = srlPidBuf[2] - '0';
    // float v   = strtof((char *)&srlPidBuf[4], NULL); // 从第4个字节开始转换为浮点数

    // e.g. {Pn 0.05}

    // switch (type) {
    //     case 'P':
    //         pidValue[num].Kp = v;
    //         break;
    //     case 'I':
    //         pidValue[num].Ki = v;
    //         break;
    //     case 'D':
    //         pidValue[num].Kd = v;
    //         break;
    //     default:
    //         break;
    // }

    srlPidFlg = 0;
    if (srlReFlag)
        serial_printf(SRL_RESRL, "> PID\n");
}

/******************************************************************
 * \brief  解析 srlCmdBuf 数据包内容
 * \note   在主循环中调用，当 srlCmdFlg 为 1 时
 * \note   支持的命令：
 *            debug -i/-o              打开/关闭调试模式
 *            srl -r <x> <str>         向串口x发送字符串
 *            led -i/-o/-t <x>         打开/关闭/切换 LEDx
 *            svo -p/-s <t> <c> <val>  设置指定时钟舵机位置/速度
 */
void misc_srlParse_cmd(void)
{
    if (!srlCmdFlg)
        return;

    u8 *cCmd    = srlCmdBuf + 1; // 正文指针
    u8 errLayer = 0;             // 错误层级，0 表示无错误，1 表示第一层错误，依此类推
    s16 arg     = 0;

    if (srlReFlag)
        serial_printf(SRL_RESRL, "SRL:  CMD: %s\n", srlCmdBuf + 1);

    if (strmatch_s("debug", cCmd, &cCmd)) {
        if (strmatch_s("-i", cCmd, &cCmd)) {
            serial_printf(SRL_RESRL, "SRL:  CMD: %s\n", srlCmdBuf + 1);
            srlReFlag = 1;
        } else if (strmatch_s("-o", cCmd, &cCmd)) {
            serial_printf(SRL_RESRL, "SRL:  CMD: %s\n", srlCmdBuf + 1);
            srlReFlag = 0;
        } else
            errLayer = 2;

    } else if (strmatch_s("srl", cCmd, &cCmd)) {
        if (strmatch_s("-r", cCmd, &cCmd)) {
            arg = (u8)strtof(cCmd, &cCmd);
            serial_printf(arg - 1, "%s\r\n", cCmd + 1);
        } else
            errLayer = 2;

    } else if (strmatch_s("led", cCmd, &cCmd)) {
        if (strmatch_s("-i", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_on(arg);
        } else if (strmatch_s("-o", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_off(arg);
        } else if (strmatch_s("-t", cCmd, &cCmd)) {
            arg = strtof(cCmd, NULL);
            led_turn(arg);
        } else
            errLayer = 2;

    } else if (strmatch_s("sm", cCmd, &cCmd)) {
        if (strmatch_s("--debug", cCmd, &cCmd)) {
            if (strmatch_s("-i", cCmd, &cCmd))
                zdtReFlag = 1;
            else if (strmatch_s("-o", cCmd, &cCmd))
                zdtReFlag = 0;
            else
                errLayer = 2;
        } else if (strmatch_s("-v", cCmd, &cCmd)) {
            u8 addr   = (u8)strtof(cCmd, &cCmd);
            float vel = (float)strtof(cCmd, &cCmd);
            u8 dir    = (vel > 0) ? 0 : 1; // 根据速度正负自动判断方向
            u8 acc    = (u8)strtof(cCmd, &cCmd);
            vel *= 10; // 开启了缩小十倍，实际输入单位为 0.1 RPM
            // while (zdtTvFlg); // 等待清零
            // Emm_V5_Vel_Control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, false);
            zdt_vel_control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, false);
        } else if (strmatch_s("-p", cCmd, &cCmd)) {
            u8 addr   = (u8)strtof(cCmd, &cCmd);
            float vel = (float)strtof(cCmd, &cCmd);
            u8 acc    = (u8)strtof(cCmd, &cCmd);
            s32 pos   = (s32)strtof(cCmd, &cCmd);
            vel *= 10;                  // 开启了缩小十倍，实际输入单位为 0.1 RPM
            u8 dir = (pos > 0) ? 0 : 1; // 根据位置正负自动判断方向
            // while (zdtTvFlg);           // 等待清零
            // Emm_V5_Pos_Control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, (u32)ABS(pos), true, false);
            zdt_pos_control(ZDT_SRL, addr, dir, (u16)ABS(vel), acc, (u32)ABS(pos), true, false);
        } else if (strmatch_s("-o", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            // Emm_V5_Stop_Now(ZDT_SRL, addr, false);
            zdt_stop_now(ZDT_SRL, addr, false);
        } else if (strmatch_s("--oso", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            // Emm_V5_Origin_Set_O(ZDT_SRL, addr, true);
            zdt_origin_set_o(ZDT_SRL, addr, true);
        } else if (strmatch_s("--otr", cCmd, &cCmd)) {
            u8 addr = (u8)strtof(cCmd, &cCmd);
            // while (zdtTvFlg); // 等待清零
            // Emm_V5_Origin_Trigger_Return(ZDT_SRL, addr, 1, false); // 1 为单圈方向回零
            zdt_origin_trigger_return(ZDT_SRL, addr, 1, false); // 1 为单圈方向回零
        } else
            errLayer = 2;

        // } else if (strmatch_s("svo", cCmd, &cCmd)) {
        //     u8 t = 0, c = 0;
        //     if (strmatch_s("-p", cCmd, &cCmd)) {
        //         t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
        //         c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
        //         arg = strtof(cCmd, NULL);      // 获取位置
        //         servo_set_pos(t, c, arg);      // 设置舵机位置
        //     } else if (strmatch_s("-s", cCmd, &cCmd)) {
        //         t   = (u8)strtof(cCmd, &cCmd); // 获取舵机时钟
        //         c   = (u8)strtof(cCmd, &cCmd); // 获取舵机通道
        //         arg = strtof(cCmd, NULL);      // 获取速度
        //         servo_set_spd(t, c, arg);      // 设置舵机速度
        //     } else if (strmatch_s("-d", cCmd, &cCmd)) {
        //         t   = (u8)strtof(cCmd, &cCmd);   // 获取舵机时钟
        //         c   = (u8)strtof(cCmd, &cCmd);   // 获取舵机通道
        //         arg = strtof(cCmd, NULL);        // 获取占空比
        //         timer_pwmOut_setDuty(t, c, arg); // 直接设置占空比
        //     } else
        //         errLayer = 2;

    } else
        errLayer = 1;

    srlCmdFlg = 0;

    if (srlReFlag && errLayer)
        serial_printf(SRL_RESRL, "SRL:  Err at layer %d\n", errLayer);
}

/* ******************** 串口解析函数 */
