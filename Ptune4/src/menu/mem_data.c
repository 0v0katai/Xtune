#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/clock.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "settings.h"
#include "config.h"
#include "menu.h"

#ifdef ENABLE_HELP
static void help_info()
{
    #if defined CP400
    info_box(15, 1, C_BLACK, "HELP",
    	"[=]: Reset to default\n"
    	"[x]: Toggle margin view\n"
    	"[y]: ROM read test\n"
    	"[z]: SDRAM write test\n"
    	"[Clear]: Close help / < Express menu");
    #elif defined CG100
    info_box(4, 1, C_BLACK, "HELP",
    	"[ON]: Reset to default\n"
    	"[|<-]: Toggle margin view\n"
    	"[->|]: ROM read test\n"
    	"[PGUP]: SDRAM write test\n"
    	"[BACK]: Close help / < Express menu");
    #elif defined CG50
    info_box(4, 1, C_BLACK, "HELP",
    	"[F1]: Reset to default\n"
    	"[F3]: Toggle margin view\n"
    	"[F5]: ROM read test\n"
    	"[F6]: SDRAM write test\n"
    	"[EXIT]: Close help / < Express menu");
    #else
    info_box(3, 1, C_BLACK, "HELP",
    	"[F1]: Reset to default\n"
    	"[F2]: Toggle SRAM read/write view\n"
    	"[F3]: Toggle margin view\n"
    	"[F5]: ROM read test\n"
    	"[F6]: SRAM read/write test\n"
    	"[EXIT]: Close help / < Express menu");
    #endif
    while (xtune_getkey().key != KEY_EXIT);
}
#endif

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
    mem_test_settings test_settings = {.byte = 0b111};

    static const i32 roR_default[] =
    {
        roR_0, roR_1, roR_2, roR_3, roR_4,
        roR_5, roR_6, roR_8, roR_10, roR_12,
        roR_14, roR_18
    };
    #if !defined CG50 && !defined CG100 && !defined CP400
    bool mode = READ;
    static const u32 raR_default[] = {raR_0, raR_1, raR_2, raR_3, raR_4, raR_5, raR_6, raR_8};
    static const u32 raW_default[] = {raW_0, raW_1, raW_2, raW_3, raW_4, raW_5, raW_6};
    #else
    static const u32 raW_TRC_default[] = {raW_TRC_3, raW_TRC_4, raW_TRC_6, raW_TRC_9};
    #endif

    #ifdef ENABLE_HELP
    set_help_function(help_info);
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
                ? roR[i] / 100 * (100 - ROM_MARGIN) / 1000
                : roR[i] / 1000);
        }
        #if defined CP400 || defined CG50 || defined CG100
        for (int i = 0; i < 4; i++)
        {
            row_print(i + RAM_DISPLAY_ROW, OFFSET_X, "TRC_%d", TRC_equivalent(i));
            row_print_color(i + RAM_DISPLAY_ROW, OFFSET_X + 10,
                margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                ? raW_TRC[i] / 100 * (100 - RAM_MARGIN) / 1000
                : raW_TRC[i] / 1000);
        }
        #else
        if (mode == READ)
            for (int i = SH4_WR_0; i <= SH4_WR_8; i++)
            {
                row_print(i + 2, 25, "raR_%d", WR_equivalent(i));
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                    ? raR[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : raR[i] / 1000);
            }
        else
            for (int i = SH4_WR_0; i <= SH4_WR_6; i++)
            {
                row_print(i + 2, 25, "raW_%d", i);
                row_print_color(i + 2, 35, margin ? C_BLUE : C_BLACK, C_WHITE, "%.3D MHz", margin
                    ? raW[i] / 100 * (100 - RAM_MARGIN) / 1000
                    : raW[i] / 1000);
            }
        #endif

        static const char *on_off[] = {"Off", "On"};
        #if defined CP400 || defined CG50 || defined CG100
        for (int i = 0; i < 3; i++)
        {
            static const char *test_settings_name[3] = {"TRC_3", "roR_10", "roR_12"};
            row_print(TEST_DISPLAY_ROW - 1 + i, OFFSET_X, "%s Check [%d]", test_settings_name[i], i);
            row_print_color(TEST_DISPLAY_ROW - 1 + i, OFFSET_X + 18, C_WHITE, C_BLACK, on_off[(test_settings.byte >> (2 - i)) & 0b1]);
        }
        #else
        for (int i = 0; i < 2; i++)
        {
            row_print(TEST_DISPLAY_ROW + i, OFFSET_X, "roR_%d Check [%d]", i ? 12 : 10, i + 1);
            row_print_color(TEST_DISPLAY_ROW + i, OFFSET_X + 18, C_WHITE, C_BLACK, on_off[(test_settings.byte >> (1 - i)) & 0b1]);
        }
        #endif

        extern void print_exception(int row, int x);
        print_exception(TEST_DISPLAY_ROW + 3, 1);

        #ifndef CP400
        fkey_action(1, "Reset");
        #if !defined CG50 && !defined CG100
        if (mode)
            fkey_button(2, "Write");
        else
            fkey_button(2, "Read");
        #endif
        if (margin)
            fkey_button(3, "Margin");
        else
            fkey_action(3, "Margin");
        fkey_menu(5, "ROM");
        fkey_menu(6, "RAM");
        #endif

        key = xtune_getkey();
        switch (key.key)
        {
            case KEY_SHIFT:
                continue;
            case KEY_MEMDATA_RESET:
                for (int i = SH4_WR_0; i <= SH4_WR_18; i++)
                    roR[i] = roR_default[i];
                #if !defined CG50 && !defined CG100 && !defined CP400
                for (int i = SH4_WR_0; i <= SH4_WR_8; i++)
                    raR[i] = raR_default[i];
                for (int i = SH4_WR_0; i <= SH4_WR_6; i++)
                    raW[i] = raW_default[i];
                #else
                for (int i = 0; i < 4; i++)
                    raW_TRC[i] = raW_TRC_default[i];
                #endif
                break;

            #if !defined CG50 && !defined CG100 && !defined CP400
            case KEY_F2:
                mode = !mode;
                break;
            #endif

            case KEY_0:
                test_settings.TRC_3_check = !test_settings.TRC_3_check;
                break;
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
                    sdram_test(test_settings.TRC_3_check);
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