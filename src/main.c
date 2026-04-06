#include <gint/gint.h>
#include <gint/hardware.h>
#include <gint/clock.h>
#include <gint/gdb.h>
#include <gint/keyboard.h>
#include <gint/display.h>
#include <gint/usb.h>
#include <gint/usb-ff-bulk.h>
#include <gint/prof.h>
#include <gint/exc.h>
#include <fxlibc/printf.h>
#include <stdlib.h>
#include <gint/mpu/cpg.h>

#include "config.h"
#include "menu.h"
#include "util.h"
#include "data.h"
#include "help.h"
#include "validate.h"

HHK_NAME("CPtune4")
HHK_AUTHOR("CalcLoverHK")
HHK_VERSION(XTUNE_STR)
HHK_DESCRIPTION("Overclocking utility for fx-CP calculator")

bool help_status = false;
int exception = 0;
Xtune_config_t config;
struct cpg_overclock_setting F1;

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
    #ifdef ENABLE_GDB
    if (!shift && key.key == KEY_ACON) {
        __asm__("trapa #42");
        __asm__("trapa #42");
    }
    #endif
    #ifdef ENABLE_HELP
    if (key.key == KEY_OPEN_HELP && !help_status)
        call_help_function();
    #endif
    if (shift) {
        if (key.key == KEY_ACON) {
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
        if (key.key == KEY_SAVE_CONFIG && !save_config()) {
            info_box(5, 1, C_RED, "ERROR",
                "Failed to write config to file!");
            xtune_getkey();
            shift = false;
        }
    }
    return false;
}

int main()
{
    #ifdef ENABLE_GDB
    gdb_start_on_exception();
    #endif

    __printf_enable_fixed();

    if (gint[HWCALC] != HARDWARE_TARGET)
        abort();

    prof_init();
    cpg_set_overclock_permanent(true);
    getkey_set_feature_function(global_getkey);

    if (!load_config())
        init_config();

    express_menu();
    return 1;
}
