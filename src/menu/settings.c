#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/config.h>
#include <stdio.h>
#include <string.h>
#include <util.h>

#include "settings.h"
#include "config.h"
#include "help.h"
#include "validate.h"
#include "mem_test.h"

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
    SELECT_AUTO_UP_PFC,
    SELECT_STARTUP_MEMORY_TEST,
    SELECT_F1_YES_NO,

    SELECT_MAX
};

typedef struct {
    u32 def;
    u32 max;
} select_range;

static void about()
{
    #if defined CP400
    info_box(14, 1, C_BLACK, "About",
        "%s %s @%07x\n"
        "w/ gint %s @%07x\n"
    	"Special thanks: Sentaro21, Lephe\n"
    	" \n"
    	"Copyright (C) 2025-2026 CalcLoverHK.\n"
        "This software is licensed under\n"
    	"MIT/Expat.",
    	ADDIN_NAME, XTUNE_VERSION, XTUNE_HASH,
    	GINT_VERSION, GINT_HASH
    );
    #else
    info_box(3, 1, C_BLACK, "About",
        "%s %s @%07x\n"
        "w/ gint %s @%07x\n"
        "Special thanks: Sentaro21, Lephe\n"
        " \n"
        "Copyright (C) 2025-2026 CalcLoverHK.\n"
    	"This software is licensed under MIT/Expat.\n"
        "This build targets %s.",
    	ADDIN_NAME, XTUNE_VERSION, XTUNE_HASH,
    	GINT_VERSION, GINT_HASH, PLATFORM);
    #endif
    
    xtune_getkey();
}

void settings_menu()
{
    key_event_t key;
    shift = false;
    i8 select = 0;
    i32 settings[SELECT_MAX] = {
        ROM_MARGIN, RAM_MARGIN, PLL_CLK_MAX,
        IFC_CLK_MAX, SFC_CLK_MAX, BFC_CLK_MAX,
        PFC_CLK_MAX, UNLOCKED_MODE, AUTO_REDUCE_WAIT,
        AUTO_UP_PFC, STARTUP_MEMORY_TEST, F1_YES_NO
    };
    static const i32 settings_def[SELECT_MAX] = {
        ROM_MARGIN_DEF, RAM_MARGIN_DEF, PLL_CLK_MAX_DEF,
        IFC_CLK_MAX_DEF, SFC_CLK_MAX_DEF, BFC_CLK_MAX_DEF,
        PFC_CLK_MAX_DEF, UNLOCKED_MODE_DEF, AUTO_REDUCE_WAIT_DEF,
        AUTO_UP_PFC_DEF, STARTUP_MEMORY_TEST_DEF, F1_YES_NO_DEF};
    static const i32 settings_max[SELECT_MAX] = {
        ROM_MARGIN_MAX, RAM_MARGIN_MAX, PLL_MAX,
        CPU_MAX, SHW_MAX, BUS_MAX,
        IO_MAX, true, true,
        true, true, true};

    #ifdef ENABLE_HELP
    set_help_function(HELP_SETUP);
    #endif

    while (true) {
        dclear(C_WHITE);
        row_title("Settings");
        for (int i = 0; i < 2; i++) {
            row_print(i + 1, 1, "R%cM Margin", i == 0 ? 'O' : 'A');
            row_print(i + 1, 25, "%d%%", config.setup[0] >> (28 - i * 4) & 0xF);
        }
        for (int i = 0; i < 5; i++) {
            static const char *option[] = {"PLL", "CPU", "SHW", "Bus", "I/O"};
            row_print(i + 3, 1, "%s Clock Max", option[i]);
            row_print(i + 3, 25, "%.3D", config.clock_max[i] / 1000);
            row_print(i + 3, 33, "MHz");
        }
        for (int i = 0; i < 5; i++) {
            static const char *option[] = {
                "Unrestricted Mode",
                "Auto Reduce Wait",
                "Auto Up PFC",
                "Startup Memory Test",
                "F1 Yes/No"
            };
            row_print(i + 8, 1, option[i]);
            row_print(i + 8, 25, config.setup[0] >> (23 - i) & 1 ? "On" : "Off");
        }
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
                select = (select + SELECT_MAX - 1) % SELECT_MAX;
                break;
            case KEY_DOWN:
                select = (select + 1) % SELECT_MAX;
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
        config.setup[0] = settings[0] << 28
                        | settings[1] << 24
                        | settings[7] << 23
                        | settings[8] << 22
                        | settings[9] << 21
                        | settings[10] << 20
                        | settings[11] << 19;
        memcpy(&config.clock_max, &settings[SELECT_PLL], sizeof(config.clock_max));
    };
}