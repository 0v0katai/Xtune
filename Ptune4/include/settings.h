#include "config.h"

extern i32 roR[12];
#if !defined CG50 && !defined CG100 && !defined CP400
extern u32 raR[8];
extern u32 raW[7];
#else
extern u32 raW_TRC[4];
#endif
extern i32 settings[];

#define ROM_MARGIN settings[0]
#define RAM_MARGIN settings[1]
#define PLL_CLK_MAX settings[2]
#define CPU_CLK_MAX settings[3]
#define SHW_CLK_MAX settings[4]
#define BUS_CLK_MAX settings[5]
#define IO_CLK_MAX settings[6]
#define UNLOCKED_MODE settings[7]
#define AUTO_REDUCE_WAIT settings[8]
#define AUTO_UP_PFC settings[9]
#define STARTUP_MEMORY_TEST settings[10]
#define F1_YES_NO settings[11]

#define PRESET_F1 preset[0]
#define PRESET_F2 preset[1]
#define PRESET_F3 preset[2]
#define PRESET_F4 preset[3]
#define PRESET_F5 preset[4]