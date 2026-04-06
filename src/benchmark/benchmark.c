#include <gint/display.h>
#include <gint/timer.h>
#include <gint/prof.h>
#include <gint/mpu/cpg.h>

#include "config.h"
#include "menu.h"
#include "validate.h"

#include "cBench/dhry.h"
#include "cBench/whet.h"

#if AZUR
# include <azur/gint/render.h>
#endif

#if CG50 || CG100 || CP400
# define RAM_ADDRESS 0xAC150000
#else
# define RAM_ADDRESS 0xA8150000
#endif

u32 tick_count(volatile bool *flag);
u32 mem_bench(u32 *mem_area, volatile bool *flag);
bool bench_flag;

static int disable_bench_flag()
{
    bench_flag = false;
    return TIMER_STOP;
}

void run_benchmark(u32 benchmark_data[], u8 PLL)
{
    bench_flag = true;
    timer_start(timer_configure(TIMER_TMU, 100000, GINT_CALL(disable_bench_flag)));
    benchmark_data[0] = tick_count(&bench_flag) / 100;
    for (int i = 0; i < 3; i++)
    {
        static const u32 address[] = {0xA0150000, RAM_ADDRESS, 0xA4150000};
        bench_flag = true;
        timer_start(timer_configure(TIMER_TMU, 50000, GINT_CALL(disable_bench_flag)));
        benchmark_data[i + 1] = mem_bench((u32 *)address[i], &bench_flag);
    }

    #if AZUR
    benchmark_data[5] = prof_exec(azrp_update());
    #endif
    int ETMU_timer = timer_configure(TIMER_ETMU, 0xFFFFFFFF, GINT_CALL_NULL);
    timer_start(ETMU_timer);
    int ETMU_start = SH7305_ETMU[ETMU_timer - 3].TCNT;
    benchmark_data[4] = prof_exec(dupdate());
    benchmark_data[8] = 32768 / (ETMU_start - SH7305_ETMU[ETMU_timer - 3].TCNT);
    timer_stop(ETMU_timer);

    #if DHRYSTONE
    benchmark_data[6] = prof_exec(dhrystone(DHRYSTONE));
    #endif

    #if WHETSTONE
    benchmark_data[7] = prof_exec(whetstone(WHETSTONE));
    #endif

    if (CPG.SSCGCR.SSEN && PLL >= 3) {
        for (int i = 0; i <= 7; i++) {
            const u32 correction = benchmark_data[i] * 256 / PLL / 1000;
            if (i <= 3)
                benchmark_data[i] -= correction;
            else
                benchmark_data[i] += correction;
        }
    }
}
