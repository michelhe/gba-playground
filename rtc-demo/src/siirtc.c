#include <tonc.h>
#include "types.h"
#include "siirtc.h"

/* Copied and altered from https://github.com/pret/pokeemerald/blob/master/src/siirtc.c */

#define STATUS_INTFE  0x02 // frequency interrupt enable
#define STATUS_INTME  0x08 // per-minute interrupt enable
#define STATUS_INTAE  0x20 // alarm interrupt enable
#define STATUS_24HOUR 0x40 // 0: 12-hour mode, 1: 24-hour mode
#define STATUS_POWER  0x80 // power on or power failure occurred

#define TEST_MODE 0x80 // flag in the "second" byte

#define ALARM_AM 0x00
#define ALARM_PM 0x80

#define OFFSET_YEAR         offsetof(struct SiiRtcInfo, year)
#define OFFSET_MONTH        offsetof(struct SiiRtcInfo, month)
#define OFFSET_DAY          offsetof(struct SiiRtcInfo, day)
#define OFFSET_DAY_OF_WEEK  offsetof(struct SiiRtcInfo, dayOfWeek)
#define OFFSET_HOUR         offsetof(struct SiiRtcInfo, hour)
#define OFFSET_MINUTE       offsetof(struct SiiRtcInfo, minute)
#define OFFSET_SECOND       offsetof(struct SiiRtcInfo, second)
#define OFFSET_STATUS       offsetof(struct SiiRtcInfo, status)
#define OFFSET_ALARM_HOUR   offsetof(struct SiiRtcInfo, alarmHour)
#define OFFSET_ALARM_MINUTE offsetof(struct SiiRtcInfo, alarmMinute)

#define INFO_BUF(info, index) (*((u8 *)(info) + (index)))

#define DATETIME_BUF(info, index) INFO_BUF(info, OFFSET_YEAR + index)
#define DATETIME_BUF_LEN (OFFSET_SECOND - OFFSET_YEAR + 1)

#define TIME_BUF(info, index) INFO_BUF(info, OFFSET_HOUR + index)
#define TIME_BUF_LEN (OFFSET_SECOND - OFFSET_HOUR + 1)

#define WR 0 // command for writing data
#define RD 1 // command for reading data

#define CMD(n) (0x60 | (n << 1))

#define CMD_RESET    CMD(0)
#define CMD_STATUS   CMD(1)
#define CMD_DATETIME CMD(2)
#define CMD_TIME     CMD(3)
#define CMD_ALARM    CMD(4)

#define SCK_HI      1
#define SIO_HI      2
#define CS_HI       4

#define DIR_0_IN    0
#define DIR_0_OUT   1
#define DIR_1_IN    0
#define DIR_1_OUT   2
#define DIR_2_IN    0
#define DIR_2_OUT   4
#define DIR_ALL_IN  (DIR_0_IN | DIR_1_IN | DIR_2_IN)
#define DIR_ALL_OUT (DIR_0_OUT | DIR_1_OUT | DIR_2_OUT)

/* GPIO port definitions */
#define GPIO_PORT_DATA        (*(vu16 *)0x80000C4)
#define GPIO_PORT_DIRECTION   (*(vu16 *)0x80000C6)
#define GPIO_PORT_READ_ENABLE (*(vu16 *)0x80000C8)

static bool8 sLocked;

static void EnableGpioPortRead()
{
    GPIO_PORT_READ_ENABLE = 1;
}

static void DisableGpioPortRead()
{
    GPIO_PORT_READ_ENABLE = 0;
}

void SiiRtcUnprotect(void)
{
    EnableGpioPortRead();
    sLocked = FALSE;
}

static int WriteCommand(u8 value)
{
    u8 i;
    u8 temp;

    for (i = 0; i < 8; i++)
    {
        temp = ((value >> (7 - i)) & 1);
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 5;
    }

    // control reaches end of non-void function
}

static int WriteData(u8 value)
{
    u8 i;
    u8 temp;

    for (i = 0; i < 8; i++)
    {
        temp = ((value >> i) & 1);
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 4;
        GPIO_PORT_DATA = (temp << 1) | 5;
    }

    // control reaches end of non-void function
}

static u8 ReadData()
{
    u8 i;
    u8 temp;
    u8 value;

    for (i = 0; i < 8; i++)
    {
        GPIO_PORT_DATA = 4;
        GPIO_PORT_DATA = 4;
        GPIO_PORT_DATA = 4;
        GPIO_PORT_DATA = 4;
        GPIO_PORT_DATA = 4;
        GPIO_PORT_DATA = 5;

        temp = ((GPIO_PORT_DATA & 2) >> 1);
        value = (value >> 1) | (temp << 7); // UB: accessing uninitialized var
    }

    return value;
}

bool8 SiiRtcGetStatus(struct SiiRtcInfo *rtc)
{
    u8 statusData;

    if (sLocked == TRUE)
        return FALSE;

    sLocked = TRUE;

    GPIO_PORT_DATA = 1;
    GPIO_PORT_DATA = 5;

    GPIO_PORT_DIRECTION = 7;

    WriteCommand(CMD_STATUS | RD);

    GPIO_PORT_DIRECTION = 5;

    statusData = ReadData();

    rtc->status = (statusData & (STATUS_POWER | STATUS_24HOUR))
                | ((statusData & STATUS_INTAE) >> 3)
                | ((statusData & STATUS_INTME) >> 2)
                | ((statusData & STATUS_INTFE) >> 1);

    GPIO_PORT_DATA = 1;
    GPIO_PORT_DATA = 1;

    sLocked = FALSE;

    return TRUE;
}


bool8 SiiRtcGetDateTime(struct SiiRtcInfo *rtc)
{
    u8 i;

    if (sLocked == TRUE)
        return FALSE;

    sLocked = TRUE;

    GPIO_PORT_DATA = SCK_HI;
    GPIO_PORT_DATA = SCK_HI | CS_HI;

    GPIO_PORT_DIRECTION = DIR_ALL_OUT;

    WriteCommand(CMD_DATETIME | RD);

    GPIO_PORT_DIRECTION = DIR_0_OUT | DIR_1_IN | DIR_2_OUT;

    for (i = 0; i < DATETIME_BUF_LEN; i++)
        DATETIME_BUF(rtc, i) = ReadData();

    INFO_BUF(rtc, OFFSET_HOUR) &= 0x7F;

    GPIO_PORT_DATA = SCK_HI;
    GPIO_PORT_DATA = SCK_HI;

    sLocked = FALSE;

    return TRUE;
}
