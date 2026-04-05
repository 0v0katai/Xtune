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

#if defined CP400
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
    shift = false;
    bool margin = false;
    #if !defined CG50 && !defined CG100 && !defined CP400
    bool mode = READ;
    #endif
    mem_test_settings test_settings = {.byte = 0b111};

    #ifdef ENABLE_HELP
    set_help_function(HELP_MEM_DATA);
    #endif

    while (true)
    {
        dclear(C_WHITE);
        row_title("Memory data %s @%07x", XTUNE_VERSION, XTUNE_HASH);
        row_print(1, 1, "ROM Margin: %d%%", ROM_MARGIN);
        #if defined CP400
        row_print(13, 1, "RAM Margin: %d%%", RAM_MARGIN);
        #else
        row_print(1, 25, "RAM Margin: %d%%", RAM_MARGIN);
        #endif
        for (int i = 0; i < 10; i++)
        {
            row_print(i + 2, 1, "roR_%d", WR_equivalent(i));
            row_print_color(i + 2, 11, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                ? config.roR[i] / 100 * (100 - ROM_MARGIN) / 1000
                : config.roR[i] / 1000);
        }
        #if defined CP400 || defined CG50 || defined CG100
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
            for (int i = SH4_WR_0; i <= SH4_WR_6; i++)
            {
                row_print(i + 2, 25, "raW_%d", i);
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                    ? config.raW[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : config.raW[i] / 1000);
            }
        #endif
        for (int i = 0; i < 2; i++) {
            row_print(TEST_DISPLAY_ROW + i, OFFSET_X, "roR_%d Check [%d]", i ? 12 : 10, i + 1);
            row_print_color(TEST_DISPLAY_ROW + i, OFFSET_X + 18, C_WHITE, C_BLACK, test_settings.byte >> (1 - i) & 1 ? "On" : "Off");
        }

        #ifndef CP400
        fkey_action(1, "Reset");
        #if !defined CG50 && !defined CG100
        fkey_button(2, mode ? "Write" : "Read");
        #endif
        fkey_toggle(3, "Margin", margin);
        fkey_menu(5, "ROM");
        fkey_menu(6, "RAM");
        #endif

        key = xtune_getkey();
        switch (key.key)
        {
            case KEY_SHIFT:
                continue;
            case KEY_MEMDATA_RESET:
                init_mem_data();
                break;

            #if !defined CG50 && !defined CG100 && !defined CP400
            case KEY_F2:
                mode = !mode;
                break;
            #endif

            case KEY_1:
                test_settings.roR_10_check = !test_settings.roR_10_check;
                if (!test_settings.roR_10_check && test_settings.roR_12_check)
                    test_settings.roR_12_check = false;
                break;
            case KEY_2:
                test_settings.roR_12_check = !test_settings.roR_12_check;
                if (!test_settings.roR_10_check && test_settings.roR_12_check)
                    test_settings.roR_10_check = true;
                break;

            case KEY_MEMDATA_MARGIN:
                margin = !margin;
                break;

            case KEY_MEMDATA_ROMTEST:
                rom_test(test_settings);
                break;
            
            case KEY_MEMDATA_RAMTEST:
                #if defined CP400
                info_box(15, 1, C_RED, "WARNING",
                    "SDRAM test may cause system errors!\n"
                    "It is highly recommended to press the\n"
                    "RESTART button after this test is\n"
                    "finished.\n"
                    "Are you sure you want to continue?\n\n");
                if (yes_no(23))
                    sdram_test(test_settings.TRC_3_check);
                #elif defined CG50 || defined CG100
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
                break;
            
            case KEY_EXIT:
                return;
        }
        shift = false;
    }
}