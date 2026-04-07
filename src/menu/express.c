#include <string.h>

#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
#include <gint/clock.h>
#include <gint/prof.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/gdb.h>

#include "util.h"
#include "validate.h"
#include "mem_test.h"
#include "settings.h"
#include "bsc.h"
#include "config.h"
#include "menu.h"
#include "data.h"
#include "help.h"

enum select_option
{
    SELECT_FLL,
    SELECT_PLL,
    SELECT_IFC,
    SELECT_SFC,
    SELECT_BFC,
    SELECT_PFC
};

#if !CP400
static void print_preset()
{
    #if CG100
    if (global.shift)
        fkey_menu(1, "Save");
    else
        fkey_action(1, "Default");
    static const char *names[] = {"Custom", "F1", "F2", "F3", "F4", "F5"};
    tab_menu(2, 5, "Current preset: %s", names[clock_get_speed()]);
    #else
    if (global.shift) {
        fkey_menu(1, "Save");
        for (int i = 2; i <= 5; i++)
            fkey_action(i, "v");
    } else {
        for (int i = 1; i <= 5; i++) {
            static const char *names[] = {"F1", "F2", "F3", "F4", "F5"};
            fkey_toggle(i, names[i - 1], i == clock_get_speed());
        }
    }
    #endif
}
#endif

static void print_express_cpg_bsc(struct cpg_overclock_setting s)
{
    #if CP400
    row_print(13, 2, "FLLFRQ: 0x%08X", s.FLLFRQ);
    row_print(13, 21, "FRQCR: 0x%08X", s.FRQCR);
    for (int i = 0; i < 4; i++)
    {
        static const char *csn_name[] = {"0", "2", "3", "5A"};
        row_print(i + 14, 2, "CS%sBCR: 0x%08X", csn_name[i], *(&(s.CS0BCR) + i));
        row_print(i + 14, 21, "CS%sWCR: 0x%08X", csn_name[i], *(&(s.CS0WCR) + i));
    }
    #else
    row_print(1, 29, "FLLFRQ:");
    row_print(2, 29, "FRQCR:");
    for (int i = 0; i < 8; i++) {
        static const char *csn_name[] = {"0", "2", "3", "5A"};
        row_print(i + 3, 29, "CS%s%cCR:", csn_name[i % 4], i < 4 ? 'B' : 'W');
    }
    for (int i = 0; i < 10; i++)
        row_print(i + 1, 38, "0x%08X", s.regs[i]);
    #endif
}

#if CP400
# define MEMTEST_DISPLAY_ROW 15
# define KEY_DISPLAY_ROW 8
# define REG_DISPLAY_X 7
# define WAIT_DISPLAY_X 13
# define SPEED_DISPLAY_X 21
#else
# define MEMTEST_DISPLAY_ROW 4
# define KEY_DISPLAY_ROW 9
# define REG_DISPLAY_X 6
# define WAIT_DISPLAY_X 11
# define SPEED_DISPLAY_X 17
#endif

static int __GetBatteryVoltage()
{
    #if CG20 || CG50
    return SYSCALL(1, 0, 0, 0, 0x1186);
    #elif CG100
    return CALL(1, 0, 0, 0, 0x8022fb9e);
    #elif CP400
    if (strncmp((const char *)0x814fffe0, "2023.0209.1500", 16) == 0)
        return CALL(0, 0, 0, 0, 0x8002a614);
    else if (strncmp((const char *)0x814fffe0, "2017.0512.1515", 16) == 0)
        return CALL(0, 0, 0, 0, 0x8002a62c);
    else
        return -1;
    #endif
}

bool benchmark_update = true;

void express_menu()
{
    key_event_t key;
    u8 select = SELECT_FLL;
    bool benchmark = false;
    u32 benchmark_data[9] = {0};
    bool update = false;
    static const char *option[] = {"FLL:", "PLL:", "IFC:", "SFC:", "BFC:", "PFC:", 0};

    dclear(C_WHITE);

    if (STARTUP_MEMORY_TEST) {
        info_box(MEMTEST_DISPLAY_ROW, 1, C_BLACK, "Start memory tests?",
            "Select option:\n"
            "[1] RAM only\n"
            "[2] ROM & RAM\n"
            "[3] None");
        while (key.key < KEY_1 || key.key > KEY_3)
            key = xtune_getkey();
        switch (key.key) {
            case KEY_2:
                rom_test();
                __attribute__((fallthrough));
            case KEY_1:
                #if CG50 || CG100 || CP400
                sdram_test(true);
                #else
                sram_test();
                #endif
                mem_data_menu();
        }
    }

    do
    {
        static struct cpg_overclock_setting s;
        cpg_get_overclock_setting(&s);
        cpg_set_overclock_setting(&s);
        bool spread_spectrum = CPG.SSCGCR.SSEN;
        bool selxm = CPG.FLLFRQ.SELXM;
        if (update && AUTO_REDUCE_WAIT)
            BSC.CS0WCR.WR = best_rom_wait(clock_freq()->Bphi_f);

        dclear(C_WHITE);
        #if HELP
        set_help_function(HELP_EXPRESS);
        #endif
        #if !CP400
        print_preset();
        fkey_menu(6, global.shift ? "Load" : "Bench");
        #endif

        row_title("%s %s @%07x %.2Dv",
            ADDIN_NAME, XTUNE_VERSION, XTUNE_HASH,
            gint_world_switch(GINT_CALL(__GetBatteryVoltage)));
        print_express_cpg_bsc(s);

        const clock_frequency_t f = *clock_freq();
        for (int i = 0; i < 6; i++)
            row_print(1 + i, REG_DISPLAY_X, i < 2 ? "x%d" : "1/%d", (&f.FLL)[i]);

        print_options(1, 1, option, select);
        row_print_color(1, WAIT_DISPLAY_X, C_WHITE, C_BLACK, selxm ? "XM 1/2" : "XM 1");
        row_print_color(2, WAIT_DISPLAY_X, spread_spectrum ? C_GREEN : C_WHITE, C_BLACK, spread_spectrum ? "SS On" : "SS Off");
        row_print_color(3, WAIT_DISPLAY_X, C_WHITE, f.Iphi_f > IFC_RED_ZONE ? C_RED : C_BLUE, "CPU");
        row_print_color(4, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "roR %d", WR_equivalent(BSC.CS0WCR.WR));
        #if CG20
        row_print_color(5, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raR %d", WR_equivalent(BSC.CS2WCR.WR));
        if (BSC.CS2WCR.WW)
            row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raW %d", BSC.CS2WCR.WW - 1);
        else
            row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "raW =R");
        #elif CG50 || CG100 || CP400
        row_print_color(5, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "TRC %d", TRC_equivalent(BSC.CS3WCR.TRC));
        row_print_color(6, WAIT_DISPLAY_X, C_WHITE, C_BLACK, "CL %d", BSC.CS3WCR.A3CL + 1);
        #endif

        #if USB
        row_print(KEY_DISPLAY_ROW, 2, "Capture");
        # if CG100
        row_print(KEY_DISPLAY_ROW, 12, "[x10^]");
        # else
        row_print(KEY_DISPLAY_ROW, 12, "[7]");
        # endif
        #endif

        #if HELP
        row_print(KEY_DISPLAY_ROW + 1, 2, "Help");
        # if CG100
        row_print(KEY_DISPLAY_ROW + 1, 12, "[CATALOG]");
        # elif CP400
        row_print(KEY_DISPLAY_ROW + 1, 12, "[8]");
        # else
        row_print(KEY_DISPLAY_ROW + 1, 12, "[4]");
        # endif
        #endif

        for (int i = 0; i < 6; i++)
        {
            row_print_color(i + 1, SPEED_DISPLAY_X, (&f.FLL_f)[i] > red_zone_defs[i]
                ? C_RED : C_BLACK, C_WHITE, "%.3D", (&f.FLL_f)[i] / 1000);
            row_print(i + 1, SPEED_DISPLAY_X + 7, "MHz");
        }

        #if !CP400
        if (!benchmark)
        #endif
        {
            static const char *description[] = {"FLL", "PLL", "CPU", "SuperHyway", "Bus", "I/O"};
            row_print(KEY_DISPLAY_ROW + 3, 2, "%s %s", description[select],
                select < SELECT_IFC ? "multiplier" : "clock divider");
            #if CP400
            if (select != SELECT_FLL)
                row_print(11, 25, "(Max %.3D MHz)", config.clock_max[select - 1] / 1000);
            row_highlight(11);
            #else
            if (select != SELECT_FLL)
                row_print(12, 33, "(Max %.3D MHz)", config.clock_max[select - 1] / 1000);
            row_highlight(12);
            #endif
        }

        if (benchmark)
        {
            if (benchmark_update)
                run_benchmark(benchmark_data, f.PLL);
            benchmark_update = false;

            #if CP400
            # define SCORE_X(i) 2
            # define SCORE_ROW(i) (19 + (i))
            # define UPDATE_ROW 23
            # define DHRYSTONE_ROW 24
            # define WHETSTONE_X 2
            # if WHETSTONE
            #  define WHETSTONE_ROW 25
            # else
            #  define WHETSTONE_ROW 24
            # endif
            #else
            # define SCORE_X(i) ((i) * 12 + 2)
            # define SCORE_ROW(i) 11
            # define UPDATE_ROW 12
            # define DHRYSTONE_ROW 13
            # define WHETSTONE_X 26
            # define WHETSTONE_ROW 13
            #endif
            
            for (int i = 0; i < 4; i++)
            {
                static const char *score_name[] = {"CPU", "ROM", "RAM", "I/O"};
                row_print(SCORE_ROW(i), SCORE_X(i), "%s %d", score_name[i], benchmark_data[i]);
            }
            row_print_color(UPDATE_ROW, 2, benchmark_data[8] + 1 < 1000000 / benchmark_data[4] ? C_RGB(0,31,31) : C_RGB(31,0,31), C_WHITE,
                "dupdate: %.2D FPS", 100000000 / benchmark_data[4]);
            
            #if AZUR
            row_print(UPDATE_ROW, 26, "azrp: %.2D FPS", benchmark_data[5], 100000000 / benchmark_data[5]);
            #endif

            #if DHRYSTONE
            row_print(DHRYSTONE_ROW, 2, "INT: %llu Dhrystone/s", DHRYSTONE * 1000000ull / benchmark_data[6]);
            #endif

            #if WHETSTONE
            row_print(WHETSTONE_ROW, WHETSTONE_X, "DBL: %d KWIPS", 100 * WHETSTONE * 1000000 / benchmark_data[7]);
            #endif

            for (int i = SCORE_ROW(0); i != WHETSTONE_ROW + 1; i++)
                row_highlight(i);
        }

        key = xtune_getkey();

        static const u8 divs_ratio[3][3] =
        {
            {2, 0, 0},
            {8, 4, 0},
            {16, 8, 8}
        };
        u32 divs[4] = {f.Iphi_div, f.Sphi_div, f.Bphi_div, f.Pphi_div};
        update = false;
        switch (key.key)
        {
            case KEY_SHIFT:
                continue; 

            case KEY_DEFAULT_PRESET:
                if (!global.no_reset && F1_YES_NO) {
                    info_box(5, 0, C_BLACK, "Caution",
                        "Reset to default preset?\n\n");
                    if (yes_no(8))
                        clock_set_speed(CLOCK_SPEED_DEFAULT);
                }
            break;
            #if CP400
            case KEY_2:
                if (global.shift)
                    config.F2 = s;
                else
                    cpg_set_overclock_setting(&config.F2);
                break;
            case KEY_3:
                if (global.shift)
                    config.F3 = s;
                else
                    cpg_set_overclock_setting(&config.F3);
                break;
            case KEY_4:
                if (global.shift)
                    config.F4 = s;
                else
                    cpg_set_overclock_setting(&config.F4);
                break;
            case KEY_5:
                if (global.shift)
                    config.F5 = s;
                else
                    cpg_set_overclock_setting(&config.F5);
                break;
            #elif CG100
            case KEY_PREVTAB:
            case KEY_NEXTTAB:
                u8 select_preset = CLOCK_SPEED_F2 - 2;
                bool stay = true;
                while (stay) {
                    tab_action(2, 5, "%-10s%s preset: F%d%10s", "|<-",
                        global.shift ? "Save" : "Set", select_preset + 2, "->|");
                    switch (xtune_getkey().key) {
                        case KEY_SHIFT:
                            continue;
                        case KEY_PREVTAB:
                            select_preset = (select_preset - 1 + 4) % 4;
                            break;
                        case KEY_NEXTTAB:
                            select_preset = (select_preset + 1) % 4;
                            break;
                        case KEY_OK:
                        case KEY_EXE:
                            if (global.shift)
                                config.presets[select_preset] = s;
                            else
                                cpg_set_overclock_setting(&config.presets[select_preset]);
                            __attribute__((fallthrough));
                        case KEY_BACK:
                            stay = false;
                    }
                }
            #else
            case KEY_F2:
            case KEY_F3:
            case KEY_F4:
            case KEY_F5:
                if (global.shift) {
                    info_box(5, 0, C_BLACK, "Caution",
                        "Save to F%d?\n\n",
                        key.key - KEY_F2 + 2);
                    if (yes_no(8)) {
                        config.presets[key.key - KEY_F2] = s;
                        global.saved = false;
                    }
                    break;
                }
                cpg_set_overclock_setting(&config.presets[key.key - KEY_F2]);
            #endif
            benchmark_update = true;
            break;

            case KEY_EXPRESS_BENCHMARK:
                if (global.shift) {
                    info_box(5, 0, C_BLACK, "Caution",
                        "Load config?\n\n");
                    if (yes_no(8)) {
                        if (!load_config()) {
                            info_box(5, 1, C_RED, "ERROR",
                                "Failed to load config file!");
                            xtune_getkey();
                        }
                    }
                }
                else
                    benchmark = !benchmark;
                break;

            case KEY_UP:
                if (select)
                    select--;
                break;
            case KEY_DOWN:
                if (select < SELECT_PFC)
                    select++;
                break;

            case KEY_EXPRESS_SELXM:
                selxm = !selxm || compute_limit(f.PLL_f);
                update = true;
                break;

            case KEY_EXPRESS_SS:
                if (f.PLL > 32)
                    break;
                CPG.SSCGCR.SSEN = !spread_spectrum;
                benchmark_update = true;
                break;
            case KEY_MUL:
            case KEY_DIV:
                bsc_modify(CS0WCR_WR_ptr, key.key == KEY_MUL ? 1 : -1);
                break;
            case KEY_PLUS:
            case KEY_MINUS:
                #if CG50 || CG100 || CP400
                bsc_modify(global.shift ? CS3WCR_CL_ptr : CS3WCR_TRC_ptr, key.key == KEY_PLUS ? 1 : -1);
                #else
                bsc_modify(global.shift ? CS2WCR_WW_ptr : CS2WCR_WR_ptr, key.key == KEY_PLUS ? 1 : -1);
                #endif
                break;

            case KEY_EXPRESS_SETTINGS:
                #if !CG100 && !CP400
                if (!global.shift)
                    break;
                #endif
                settings_menu();
                break;
            case KEY_EXPRESS_BSC:
                bsc_menu();
                break;
            case KEY_EXPRESS_MEMDATA:
                mem_data_menu();
                break;

            case KEY_LEFT:
                static const u16 min[6] = {450, 1, 64, 64, 64, 64};
                if ((&f.FLL)[select] == min[select])
                    break;
                if (select == SELECT_FLL) {
                    u16 new_FLL = f.FLL - (global.shift ? 50 : 1);
                    if (new_FLL < min[SELECT_FLL])
                        new_FLL = min[SELECT_FLL];
                    CPG.FLLFRQ.FLF = new_FLL;
                }
                else if (select == SELECT_PLL)
                    CPG.FRQCR.STC--;
                else
                {
                    const u8 check = select - 2;
                    for (int i = check + 1; i <= 3; i++)
                        if (divs[check] == divs[i])
                            divs[i] <<= 1;
                    divs[check] <<= 1;
                    for (int i = check - 1; i >= 0; i--)
                        if (divs[check] / divs[i] > divs_ratio[check - 1][i])
                            divs[i] <<= 1;
                }
                if (select <= SELECT_PLL)
                    divs[SELECT_PFC - 2] >>= auto_up_PFC();
                update = true;
                break;
            case KEY_RIGHT:
                const u16 max[6] = {2047, 64 >> spread_spectrum,
                                    2, 2, 2, 2};
                if ((&f.FLL)[select] == max[select])
                    break;
                if (select == SELECT_FLL) {
                    const u8 increment = global.shift ? 50 : 1;
                    u16 new_FLL = f.FLL + increment;
                    if (new_FLL > max[SELECT_FLL])
                        new_FLL = max[SELECT_FLL];
                    if (!compute_limit(increment * (32768 >> selxm) * f.PLL))
                        CPG.FLLFRQ.FLF = new_FLL;
                    else
                        break;
                }
                else if (select == SELECT_PLL)
                {
                    if (!compute_limit(f.FLL_f))
                        CPG.FRQCR.STC++;
                    else
                        break;
                }
                else
                {
                    const u8 check = select - 2;
                    const u32 limit[4] = {IFC_CLK_MAX, SFC_CLK_MAX, BFC_CLK_MAX, PFC_CLK_MAX};
                    if (!UNLOCKED_MODE && ((&f.Iphi_f)[check] << 1) > limit[check])
                        break;
                    for (int i = check - 1; i >= 0; i--)
                        if (divs[check] == divs[i])
                            divs[i] >>= 1;
                    divs[check] >>= 1;
                    for (int i = check + 1; i <= 3; i++)
                        if (divs[i] / divs[check] > divs_ratio[i - 1][check])
                            divs[i] >>= 1;
                }
                if (select <= SELECT_PLL)
                    divs[SELECT_PFC - 2] <<= auto_down_PFC();
                update = true;
                break;
        }
        global.byte &= 0b100;
        if (update)
        {
            benchmark_update = true;
            CPG.FRQCR.lword &= 0xFF000000;
            for (int i = 0; i < 4; i++)
            {
                static const u8 field[4] = {20, 12, 8, 0};
                while ((divs[i] >>= 1) != 1)
                    CPG.FRQCR.lword += 1 << field[i];
            }
            cpg_compute_freq();
            u32 Bphi_f = clock_freq()->Bphi_f << !selxm;
            const u8 new_CS0WCR_WR = best_rom_wait(Bphi_f);
            if (!UNLOCKED_MODE && new_CS0WCR_WR > BSC.CS0WCR.WR)
                BSC.CS0WCR.WR = new_CS0WCR_WR;
            #if CG50 || CG100 || CP400
            const u8 new_CS3WCR_TRC = best_TRC(Bphi_f);
            if (!UNLOCKED_MODE && (new_CS3WCR_TRC > BSC.CS3WCR.TRC || AUTO_REDUCE_WAIT))
                BSC.CS3WCR.TRC = new_CS3WCR_TRC;
            #else
            const u8 new_CS2WCR_WR = best_ram_read(Bphi_f);
            const u8 new_CS2WCR_WW = best_ram_write(Bphi_f);
            if (!UNLOCKED_MODE && (new_CS2WCR_WR > BSC.CS2WCR.WR || AUTO_REDUCE_WAIT))
                BSC.CS2WCR.WR = new_CS2WCR_WR;
            if (!UNLOCKED_MODE && (new_CS2WCR_WW > BSC.CS2WCR.WW || AUTO_REDUCE_WAIT))
                BSC.CS2WCR.WW = new_CS2WCR_WW;
            #endif
            CPG.FLLFRQ.SELXM = selxm;
        }
    #if CG100
    } while (key.key != KEY_HOME);
    #elif CP400
    } while (key.key != KEY_EXIT);
    #else
    } while (true);
    #endif
}
