
#include <stdio.h>
#include <tonc.h>

#include "types.h"
#include "siirtc.h"

u32 bcd2dec(u8 bcd)
{
    if (bcd > 0x9F)
        return 0xFF;

    if ((bcd & 0xF) <= 9)
        return (10 * ((bcd >> 4) & 0xF)) + (bcd & 0xF);
    else
        return 0xFF;
}

const char *weekday2str(int weekday)
{
    switch (weekday - 1)
    {
    case 0:
        return "Mon";
    case 1:
        return "Tue";
    case 2:
        return "Wed";
    case 3:
        return "Thu";
    case 4:
        return "Fri";
    case 5:
        return "Sat";
    case 6:
        return "Sun";
    default:
        return "INVALID DAY";
    }
}

int main(void)
{

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0;

    tte_init_se_default(0, BG_CBB(0) | BG_SBB(31));
    tte_init_con();

    struct SiiRtcInfo rtc = {0};

    SiiRtcUnprotect();
    bool8 ok = SiiRtcGetStatus(&rtc);

    if (!ok)
    {
        tte_printf("${es} Failed to get RTC status");
        goto error;
    }

    while (1)
    {
        vid_vsync();
        ok = SiiRtcGetDateTime(&rtc);
        if (!ok)
        {
            tte_printf("#{es} Failed to get RTC datetime");
            goto error;
        }
        tte_printf("#{es;P:10,70} %s,", weekday2str(bcd2dec(rtc.dayOfWeek)));
        tte_printf("#{P:10,80}  %d/%d/%d %02d:%02d:%02d",
                   bcd2dec(rtc.month),
                   bcd2dec(rtc.day),
                   bcd2dec(rtc.year) + 2000,
                   bcd2dec(rtc.hour),
                   bcd2dec(rtc.minute),
                   bcd2dec(rtc.second));
    }

error:
    while (1)
        ;

    return 0;
}