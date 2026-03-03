#include <gint/gint.h>
#include <gint/hardware.h>
#include <gint/clock.h>
#include <gint/gdb.h>
#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>
#include <gint/prof.h>
#include <fxlibc/printf.h>
#include <stdlib.h>

#include "config.h"
#include "menu.h"
#include "util.h"
#include "data.h"

HHK_NAME("CPtune4")
HHK_AUTHOR("CalcLoverHK")
HHK_VERSION(XTUNE_STR)
HHK_DESCRIPTION("Overclocking utility for fx-CP calculator")

bool help_status = false;
struct cpg_overclock_setting preset[5];

static bool global_getkey(key_event_t key)
{
    if (key.key == KEY_SHIFT)
        shift = !shift;
    #ifdef ENABLE_USB
    if (key.key == KEY_ENABLE_USB)
    {
        shift = false;
        if (!usb_is_open())
        {
            usb_interface_t const *interfaces[] = {&usb_ff_bulk, NULL};
            usb_open(interfaces, GINT_CALL_NULL);
            usb_open_wait();
        }
        usb_fxlink_screenshot(true);
        return true;
    }
    #endif
    #ifdef ENABLE_HELP
    if (key.key == KEY_OPEN_HELP && !help_status)
        call_help_function();
    if (shift && key.key == KEY_ACON)
    {
        # if defined CG50 && !defined TARGET_FXCG50_FASTLOAD
        info_box(5, 1, C_BLACK, "Caution",
            "Poweroff function is disabled in this build.\n"
            "Please return to the main menu before\n"
            "turning off your calculator.");
        xtune_getkey();
        # else
        gint_poweroff(true);
        # endif
        shift = false;
    }
    #endif
    return false;
}

int main()
{
    #ifdef ENABLE_GDB
    gdb_start_on_exception();
    __asm__("trapa #42");
    #endif

    __printf_enable_fixed();

    if (gint[HWCALC] != HARDWARE_TARGET)
        abort();

    prof_init();
    cpg_set_overclock_permanent(true);
    gint_setrestart(true);
    getkey_set_feature_function(global_getkey);

    struct cpg_overclock_setting s;
    cpg_get_overclock_setting(&s);
    for (int i = CLOCK_SPEED_F1; i <= CLOCK_SPEED_F5; i++)
    {
        clock_set_speed(i);
        cpg_get_overclock_setting(&preset[i-1]);
    }
    cpg_set_overclock_setting(&s);
    load_config();

    express_menu();
    return 1;
}
