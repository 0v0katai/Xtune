#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#define ROM_WAIT(CS0WCR) (((CS0WCR) >> 7) & 0b1111)

#if defined CP400
# define SELECT_DISPLAY_ROW 36
#else
# define SELECT_DISPLAY_ROW 13
#endif

static void print_ROM_read_select(u32 *ROM_read_area)
{
    row_clear(SELECT_DISPLAY_ROW);
    row_print(SELECT_DISPLAY_ROW, 1, "ROM select: 0x%08X", ROM_read_area);
}

static void rom_read_test()
{
    struct cpg_overclock_setting s;
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    cpg_get_overclock_setting(&s);
    u32 *read_area;

    if (!config.ROM_read_addr) {
        /* Slowest ROM read area search */
        int FLF_max = 0;
        print_ROM_read_select(ROM_BASE);
        for (int FLF = ROM_SEARCH_FLF_START; FLF < 2048; FLF++)
        {
            BSC.CS0WCR.WR = ROM_WAIT(s.CS0WCR);
            if (read_address(FLF, ROM_BASE))
                break;
            FLF_max = FLF;
            row_print(1, 1, "%.3D MHz", clock_freq()->Bphi_f / 1000);
            dupdate();
            row_clear(1);
        }

        read_area = ROM_BASE;
        for (int i = 0; i < 512; i++)
        {
            BSC.CS0WCR.WR = ROM_WAIT(s.CS0WCR);
            if (read_address(FLF_max, read_area)) {
                FLF_max -= 2;
                config.ROM_read_addr = read_area;
            }
            print_ROM_read_select(config.ROM_read_addr);
            row_print_color(SELECT_DISPLAY_ROW, 25, C_RED, C_WHITE, "0x%08X", read_area);
            dupdate();
            row_clear(SELECT_DISPLAY_ROW);
            read_area += 0x10000 >> 2;
        }
    }

    read_area = config.ROM_read_addr;
    print_ROM_read_select(read_area);
    for (int i = SH4_WR_0; i <= SH4_WR_12; i++) {
        if (i == SH4_WR_10 && !TEST_ROR_10_12) {
            config.roR[SH4_WR_10] = config.roR[SH4_WR_8] / 100 * (123 - ROM_MARGIN);
            config.roR[SH4_WR_12] = config.roR[SH4_WR_10] / 100 * (118 - ROM_MARGIN);
            break;
        }
        s.FRQCR = SH4_FRQCR(6 + i * 2, SH4_DIV_4, SH4_DIV_4, SH4_DIV_4, SH4_DIV_32);
        s.CS0WCR = 0x000005C0;
        cpg_set_overclock_setting(&s);
        u32 Bphi_f;
        for (int FLF = roR_defs[i] / (PLL(6) + i * 2 + 1) / 4096; FLF < 2048; FLF++) {
            BSC.CS0WCR.WR = i;
            if (read_address(FLF, read_area))
                break;
            Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 1, "roR_%d", WR_equivalent(i));
            row_print(2 + i, 11, "%.3D MHz", Bphi_f / 1000);
            dupdate();
        }
        config.roR[i] = Bphi_f;
    }
}

void rom_test()
{
    dclear(C_WHITE);
    row_title("ROM Test");

    CPG.SSCGCR.SSEN = false;
    cpu_sr_t SR = cpu_getSR();
    cpu_setSR((cpu_sr_t)(SR.lword | 0xf0));
    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    rom_read_test();
    cpg_set_overclock_setting(&s0);
    cpu_setSR(SR);
}