#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#if !defined CG50 && !defined CG100 && !defined CP400
# define RAM_WAIT(CS2WCR) (((CS2WCR) >> 7) & 0b1111)

static void print_RAM_read_select(u32 *RAM_read_area)
{
    row_clear(14);
    row_print(14, 1, "RAM read select: 0x%08X", RAM_read_area);
}

static void ram_read_test()
{
    struct cpg_overclock_setting s;
    clock_set_speed(CLOCK_SPEED_DEFAULT);
    cpg_get_overclock_setting(&s);
    u32 *read_area;

    if (!config.SRAM_read_addr) {
        /* Slowest RAM read area search */

        int FLF_max;
        for (int FLF = RAM_SEARCH_FLF_START; FLF < 2048; FLF++)
        {
            BSC.CS2WCR.WR = RAM_WAIT(s.CS2WCR);
            if (read_address(FLF, SRAM_BASE))
                break;
            FLF_max = FLF;
            print_RAM_read_select(SRAM_BASE);
            row_print(1, 1, "%.3D MHz", clock_freq()->Bphi_f / 1000);
            dupdate();
            row_clear(1);
        }
        read_area = SRAM_BASE;
        for (int i = 0; i < 124; i++)
        {
            BSC.CS2WCR.WR = RAM_WAIT(s.CS2WCR);
            if (read_address(FLF_max, read_area))
            {
                FLF_max -= 2;
                config.SRAM_read_addr = read_area;
            }
            print_RAM_read_select(config.SRAM_read_addr);
            row_print_color(14, 30, C_RED, C_WHITE, "0x%08X", read_area);
            dupdate();
            row_clear(14);
            read_area += 0x1000 >> 2;
        }
    }

    read_area = config.SRAM_read_addr;
    print_RAM_read_select(read_area);
    for (int i = SH4_WR_0; i <= SH4_WR_8; i++) {
        s.FRQCR = SH4_FRQCR(8 + i * 3, SH4_DIV_4, SH4_DIV_4, SH4_DIV_4, SH4_DIV_32);
        cpg_set_overclock_setting(&s);
        for (int FLF = raR_defs[i] / (PLL(8) + i * 3 + 1) / 4096; FLF < 2048; FLF++)
        {
            BSC.CS2WCR.WR = i;
            if (read_address(FLF, read_area))
                break;
            const u32 Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 25, "raR_%d", WR_equivalent(i));
            row_print(2 + i, 35, "%.3D MHz", Bphi_f / 1000);
            config.raR[i] = Bphi_f;
            dupdate();
        }
    }
    clock_set_speed(CLOCK_SPEED_DEFAULT);
}

static void ram_write_test()
{
    u32 temp[WRITE_N];
    u32 *write_area = NON_CACHE(temp);

    for (int i = 0; i <= SH4_WR_8; i++)
        row_clear(i + 2);
    row_clear(14);
    row_print(14, 1, "RAM write select: 0x%08X", write_area);
    struct cpg_overclock_setting s;
    BSC.CS2WCR.WR = SH4_WR_18;
    cpg_get_overclock_setting(&s);
    
    for (int i = SH4_WR_0; i <= SH4_WR_6; i++)
    {
        s.FRQCR = SH4_FRQCR(8 + i * 3, SH4_DIV_4, SH4_DIV_4, SH4_DIV_4, SH4_DIV_32);
        cpg_set_overclock_setting(&s);
        for (int FLF = raW_defs[i] / (PLL(8) + i * 3 + 1) / 4096; FLF < 2048; FLF++)
        {
            BSC.CS2WCR.WW = i + 1;
            if (write_address(FLF, write_area))
                break;
            const u32 Bphi_f = clock_freq()->Bphi_f;
            row_clear(2 + i);
            row_print(2 + i, 25, "raW_%d", i);
            row_print(2 + i, 35, "%.3D MHz", Bphi_f / 1000);
            config.raW[i] = Bphi_f;
            dupdate();
        }
    }
}

void sram_test()
{
    dclear(C_WHITE);
    row_title("SRAM Test");

    CPG.SSCGCR.SSEN = false;
    cpu_sr_t SR = cpu_getSR();
    cpu_setSR((cpu_sr_t)(SR.lword | 0xf0));
    struct cpg_overclock_setting s0;
    cpg_get_overclock_setting(&s0);
    ram_read_test();
    ram_write_test();
    cpg_set_overclock_setting(&s0);
    cpu_setSR(SR);
}
#endif