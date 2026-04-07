#include <gint/clock.h>
#include <gint/timer.h>
#include <gint/mpu/cpg.h>
#include <gint/mpu/bsc.h>

#include "settings.h"
#include "validate.h"
#include "config.h"

#define CS2WCR_DEFAULT SH4_WR_2
#define CS0WCR_DEFAULT SH4_WR_3

bool compute_limit(int PLL_add) {
    if (UNLOCKED_MODE)
        return false;

    const clock_frequency_t *freq = clock_freq();
    const u32 PLL_f = freq->PLL_f + PLL_add;
    if (PLL_f > PLL_CLK_MAX)
        return true;

    for (int i = 0; i < 4; i++)
        if (PLL_f / freq->divs[i] > config.clock_max[i])
            return i == 3 ? freq->divs[i] == 64 : true;

    return false;
}

unsigned int best_rom_wait(u32 Bphi_f) {
    for (int i = SH4_WR_0; i <= SH4_WR_12; i++)
        if (Bphi_f <= config.roR[i] / 100 * (100 - ROM_MARGIN))
            return i;
    return SH4_WR_14;
}

#if CG50 || CG100 || CP400
unsigned int best_TRC(u32 Bphi_f) {
    for (int i = SH4_TRC_3; i <= SH4_TRC_6; i++)
        if (Bphi_f <= config.TRC[i] / 100 * (100 - RAM_MARGIN))
            return i;
    return SH4_TRC_9;
}
#else
unsigned int best_ram_read(u32 Bphi_f)
{
    for (int i = SH4_WR_0; i <= SH4_WR_8; i++)
        if (Bphi_f <= config.raR[i] / 100 * (100 - RAM_MARGIN))
            return i;
    return SH4_WR_10;
}

unsigned int best_ram_write(u32 Bphi_f)
{
    for (int i = SH4_WW_0; i <= SH4_WW_5; i++)
        if (Bphi_f <= config.raW[i - 1] / 100 * (100 - RAM_MARGIN))
            return i;
    return SH4_WW_6;
}
#endif

int BCR_equivalent(u8 reg_value)
{
    static const u8 bcr_wait[] = {0, 1, 2, 4, 6, 8, 10, 12};
    return bcr_wait[reg_value];
}

int WR_equivalent(u8 reg_value)
{
    static const u8 wr_wait[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 18, 24};
    return wr_wait[reg_value];
}

int TRC_equivalent(u8 reg_value)
{
    return (reg_value * (reg_value + 1) + 6) / 2;
}

bool auto_up_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    return AUTO_UP_PFC && freq->Pphi_f << 1 < PFC_CLK_MAX && freq->Pphi_div != freq->Bphi_div;
}

bool auto_down_PFC()
{
    cpg_compute_freq();
    const clock_frequency_t *freq = clock_freq();
    return !UNLOCKED_MODE && freq->Pphi_f > PFC_CLK_MAX && freq->Pphi_div != 64;
}
