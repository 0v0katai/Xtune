#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/config.h>
#include <stdio.h>
#include <util.h>

#include "settings.h"
#include "config.h"
#include "validate.h"
#include "mem_test.h"

#ifdef ENABLE_HELP
static void help_info()
{
    #if defined CP400
    info_box(15, 6, C_BLACK, "HELP"
    	"[=]: Reset to default",
    	"[-][+]: -/+ option value (1 MHz)",
    	"[LEFT][RIGHT]: -/+ option value (0.1 MHz)",
    	"[UP][DOWN]: Select option",
    	"[/]: About this add-in",
    	"[Clear]: Close help / < Express menu");
    #elif defined CG100
    info_box(4, 6, C_BLACK, "HELP",
    	"[ON]: Reset to default",
    	"[+][-]: +/- option value (1 MHz)",
    	"[LEFT][RIGHT]: -/+ option value (0.1 MHz)",
    	"[UP][DOWN]: Select option",
    	"[PGUP]: About this add-in",
    	"[BACK]: Close help / < Express menu");
    #else
    info_box(3, 8, C_BLACK, "HELP",
    	"[F1]: Reset to default",
    	"[F2][+]: Increase option value (1 MHz)",
    	"[F3][-]: Decrease option value (0.1 MHz)",
    	"[LEFT][RIGHT]: -/+ option value (0.1 MHz)",
    	"[UP][DOWN]: Select option",
    	"[F6]: About this add-in",
        "",
    	"[EXIT]: Close help / < Express menu");
    #endif
    while (xtune_getkey().key != KEY_EXIT);
}
#endif

i32 settings[] =
{
    ROM_MARGIN_DEF,
    RAM_MARGIN_DEF,
    PLL_CLK_MAX_DEF,
    IFC_CLK_MAX_DEF,
    SFC_CLK_MAX_DEF,
    BFC_CLK_MAX_DEF,
    PFC_CLK_MAX_DEF,
    UNLOCKED_MODE_DEF,
    AUTO_REDUCE_WAIT_DEF,
    AUTO_UP_PFC_DEF
};

enum select_option
{
    SELECT_ROM,
    SELECT_RAM,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC,
    SELECT_UNLOCKED_MODE,
    SELECT_AUTO_REDUCE_WAIT,
    SELECT_AUTO_UP_PFC
};

static void about()
{
    char gint_str[32];
    sprintf(gint_str, "w/ gint %s @%07x",
        GINT_VERSION,
        GINT_HASH);
    #if defined CP400
    info_box(14, 9, C_BLACK, "About",
    	"",
        XTUNE_STR,
        gint_str,
    	"Special thanks: Sentaro21, Lephe",
    	"",
    	"Copyright (C) 2025-2026 CalcLoverHK.",
        "This software is licensed under",
    	"MIT/Expat.",
    	"");
    #else
    info_box(3, 9, C_BLACK, "About",
    	"",
    	XTUNE_STR,
        gint_str,
        "Special thanks: Sentaro21, Lephe",
        "",
        "Copyright (C) 2025-2026 CalcLoverHK.",
    	"This software is licensed under MIT/Expat.",
        "This build targets " PLATFORM ".",
    	"");
    #endif
    
    xtune_getkey();
}

void settings_menu()
{
    key_event_t key;
    shift = false;
    i8 select = 0;
    static const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
    static const char *off_on[] = {"Off", "On"};
    static const i32 settings_def[] =
        {ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF,
         IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF,
         PFC_CLK_MAX_DEF, false, true, true};
    static const i32 settings_max[] =
        {ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX, CPU_MAX, SHW_MAX, BUS_MAX, IO_MAX, true, true, true};

    static const i8 select_max = SELECT_AUTO_UP_PFC + 1;

    #ifdef ENABLE_HELP
    set_help_function(help_info);
    #endif

    while (true)
    {
        dclear(C_WHITE);
        row_title("Settings");
        row_print(1, 1, "ROM Margin");
        row_print(1, 25, "%d%%", ROM_MARGIN);
        row_print(2, 1, "RAM Margin");
        row_print(2, 25, "%d%%", RAM_MARGIN);
        for (int i = 0; i < 5; i++)
        {
            row_print(i + 3, 1, "%s Clock Max", option[i]);
            row_print(i + 3, 25, "%.3D", settings[i + 2] / 1000);
            row_print(i + 3, 33, "MHz");
        }
        row_print(8, 1, "Unrestricted mode");
        row_print(9, 1, "Auto Reduce Wait");
        row_print(10, 1, "Auto Up PFC");
        for (int i = 0; i < 3; i++)
            row_print(8 + i, 25, off_on[settings[7 + i]]);

        row_highlight(select + 1);

        #ifndef CP400
        fkey_action(1, "Reset");
        # ifndef CG100
        fkey_action(2, select <= SELECT_PFC ? "+" : "On");
        fkey_action(3, select <= SELECT_PFC ? "-" : "Off");
        # endif
        fkey_button(6, "About");
        #endif

        key = xtune_getkey();

        i32 modify = 0;
        bool scale = false;
        switch (key.key)
        {
            case KEY_SHIFT:
                continue;
            case KEY_UP:
                select = (select + select_max - 1) % select_max;
                break;
            case KEY_DOWN:
                select = (select + 1) % select_max;
                break;

            case KEY_SETTINGS_RESET:
                settings[select] = settings_def[select];
                break;

            #if !defined CG100 && !defined CP400
            case KEY_F2:
            #endif
            case KEY_PLUS:
                scale = true;
                __attribute__((fallthrough));
            case KEY_RIGHT:
                modify++;
                break;
            #if !defined CG100 && !defined CP400
            case KEY_F3:
            #endif
            case KEY_MINUS:
                scale = true;
                __attribute__((fallthrough));
            case KEY_LEFT:
                modify--;
                break;

            case KEY_SETTINGS_ABOUT:
                about();
                break;

            case KEY_EXIT:
                return;
        }
        shift = false;

        if (select >= SELECT_PLL)
        {
            modify *= 100 * 1000;
            if (scale)
                modify *= 10;
        }
        settings[select] += modify;
        if (settings[select] > settings_max[select])
            settings[select] = settings_max[select];
        if (settings[select] < 0)
            settings[select] = 0;
    };
}