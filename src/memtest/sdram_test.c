#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>
#include <gint/clock.h>
#include <gint/display.h>

#include "settings.h"
#include "validate.h"
#include "mem_test.h"
#include "util.h"
#include "config.h"

#if defined CG50 || defined CG100 || defined CP400

#if defined CP400
# define TRC_DISPLAY_ROW 14
# define TRC_DISPLAY_X 1
# define SELECT_DISPLAY_ROW 36
#else
# define TRC_DISPLAY_ROW 2
# define TRC_DISPLAY_X 25
# define SELECT_DISPLAY_ROW 14
#endif

static void print_SDRAM_speed(u32 Bphi_f, u8 TRC)
{
    row_clear(TRC_DISPLAY_ROW + TRC);
    row_print(TRC_DISPLAY_ROW + TRC, TRC_DISPLAY_X, "TRC_%d", TRC_equivalent(TRC));
    row_print(TRC_DISPLAY_ROW + TRC, TRC_DISPLAY_X + 10, "%.3D MHz", Bphi_f / 1000);
}

static void ram_write_test()
{
    u32 temp[WRITE_N];
    u32 *write_area = NON_CACHE(temp);
    struct cpg_overclock_setting s0, s;
    cpg_get_overclock_setting(&s0);

    row_clear(SELECT_DISPLAY_ROW);
    row_print(SELECT_DISPLAY_ROW, 1, "RAM select: 0x%08X", write_area);
    s = s0;
    s.FLLFRQ = (SH4_SELXM << 14) + 600;
    s.FRQCR = SH4_FRQCR(24, SH4_DIV_4, SH4_DIV_4, SH4_DIV_4, SH4_DIV_32);
    cpg_set_overclock_setting(&s);
    
    int FLF_max = 600;
    u32 Bphi_f;
    for (int TRC = SH4_TRC_3; TRC <= SH4_TRC_9; TRC++)
    {
        for (int FLF = FLF_max; FLF < 2048; FLF++)
        {
            BSC.CS3WCR.TRC = TRC;
            if (write_address(FLF, write_area))
            {
                FLF_max = FLF;
                for (int trial = 1; trial <= 100; trial++)
                {
                    BSC.CS3WCR.TRC = TRC;
                    if (write_address(FLF_max, write_area))
                    {
                        trial = 0;
                        FLF_max--;
                        continue;
                    }
                    Bphi_f = clock_freq()->Bphi_f;
                    row_print(TRC_DISPLAY_ROW - 1, TRC_DISPLAY_X, "Trial %d", trial);
                    row_print_color(TRC_DISPLAY_ROW - 1, TRC_DISPLAY_X + 10, C_RED, C_WHITE, "%.3D MHz", Bphi_f / 1000);
                    dupdate();
                    row_clear(TRC_DISPLAY_ROW - 1);
                }
                config.TRC[TRC] = Bphi_f;
                FLF = 2048;
            }
            print_SDRAM_speed(clock_freq()->Bphi_f, TRC);
            dupdate();
        }
    }
    cpg_set_overclock_setting(&s0);
    for (int i = 2; i >= 0; i--)
        if (config.TRC[i] > config.TRC[i + 1])
            config.TRC[i] = config.TRC[i + 1];
    BFC_CLK_MAX = config.TRC[SH4_TRC_9] / 100 * (100 - RAM_MARGIN);
}

void sdram_test() {
    dclear(C_WHITE);
    row_title("SDRAM Test");

    CPG.SSCGCR.SSEN = false;
    cpu_sr_t SR = cpu_getSR();
    cpu_setSR((cpu_sr_t)(SR.lword | 0xf0));
    ram_write_test();
    cpu_setSR(SR);
}

#endif