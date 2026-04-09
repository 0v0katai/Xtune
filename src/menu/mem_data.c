#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "settings.h"
#include "config.h"
#include "help.h"
#include "menu.h"

#if CP400
# define RAM_DISPLAY_ROW 14
# define OFFSET_X 1
# define TEST_DISPLAY_ROW 20
#else
# define RAM_DISPLAY_ROW 2
# define OFFSET_X 25
# define TEST_DISPLAY_ROW 10
#endif

void mem_data_menu()
{
    key_event_t key;
    global.shift = false;
    bool margin = false;
    #if !CG50 && !CG100 && !CP400
    bool mode = READ;
    #endif

    #if HELP
    set_help_function(HELP_MEM_DATA);
    #endif

    while (true) {
        dclear(C_WHITE);
        row_title("Memory data %s @%07x", XTUNE_VERSION, XTUNE_HASH);
        row_print(1, 1, "ROM %d%% / 0x%08X", ROM_MARGIN, config.ROM_read_addr);
        #if CP400
        row_print(13, 1, "SDRAM %d%%", RAM_MARGIN);
        #elif CG50 || CG100
        row_print(1, 25, "SDRAM %d%%", RAM_MARGIN);
        #else
        row_print(1, 25, "SRAM %d%% / 0x%08X", RAM_MARGIN, config.SRAM_read_addr);
        #endif
        for (int i = 0; i < 10; i++)
        {
            row_print(i + 2, 1, "roR_%d", WR_equivalent(i));
            row_print_color(i + 2, 11, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                ? config.roR[i] / 100 * (100 - ROM_MARGIN) / 1000
                : config.roR[i] / 1000);
        }
        #if CP400 || CG50 || CG100
        for (int i = 0; i < 4; i++)
        {
            row_print(i + RAM_DISPLAY_ROW, OFFSET_X, "TRC_%d", TRC_equivalent(i));
            row_print_color(i + RAM_DISPLAY_ROW, OFFSET_X + 10,
                margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                ? config.TRC[i] / 100 * (100 - RAM_MARGIN) / 1000
                : config.TRC[i] / 1000);
        }
        #else
        if (mode == READ)
            for (int i = SH4_WR_0; i <= SH4_WR_8; i++)
            {
                row_print(i + 2, 25, "raR_%d", WR_equivalent(i));
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                    ? config.raR[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : config.raR[i] / 1000);
            }
        else
            for (int i = SH4_WW_0; i <= SH4_WW_6; i++)
            {
                row_print(i + 1, 25, "raW_%d", i - 1);
                row_print_color(i + 1, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                    ? config.raW[i - 1] / 100 * (100 - RAM_MARGIN) / 1000
                    : config.raW[i - 1] / 1000);
            }
        #endif

        if (global.shift) {
            fkey_button(1, "Save");
        } else {
            fkey_button(1, "Reset");
            #if !CG50 && !CG100 && !CP400
            fkey_button(2, mode ? "Write" : "Read");
            #endif
            fkey_toggle(3, "Margin", margin);
            fkey_action(5, "ROM");
            fkey_action(6, "RAM");
        }

        key = xtune_getkey();
        switch (key.key)
        {
            case KEY_SHIFT:
                continue;
            case KEY_MEMDATA_RESET:
                if (!global.no_reset) {
                    init_mem_data();
                    global.saved = false;
                }
                break;

            #if !CG50 && !CG100 && !CP400
            case KEY_F2:
                mode = !mode;
                break;
            #endif

            case KEY_MEMDATA_MARGIN:
                margin = !margin;
                break;

            case KEY_MEMDATA_ROMTEST:
                rom_test();
                global.saved = false;
                break;
            
            case KEY_MEMDATA_RAMTEST:
                #if CP400
                info_box(15, 1, C_RED, "WARNING",
                    "SDRAM test may cause system errors!\n"
                    "It is highly recommended to press the\n"
                    "RESTART button after this test is\n"
                    "finished.\n"
                    "Are you sure you want to continue?\n\n");
                if (yes_no(23))
                    sdram_test();
                #elif CG50 || CG100
                info_box(4, 1, C_RED, "WARNING",
                    "SDRAM test may cause system errors!\n"
                    "It is highly recommended to press the RESTART\n"
                    "button after this test is finished.\n"
                    "Are you sure you want to continue?\n");
                if (yes_no(10))
                    sdram_test();
                #else
                sram_test();
                #endif
                global.saved = false;
                break;
            
            case KEY_EXIT:
                return;
        }
        global.byte &= 0b100;
    }
}