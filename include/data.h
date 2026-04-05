#pragma once

#include <gint/clock.h>
#include <gint/defs/types.h>

typedef struct {
    u32 hash;
    char model[4];
    array_union(u32, setup,
        u8 ROM_margin      :  4;
        u8 RAM_margin      :  4;
        bool unlocked_mode :  1;
        bool reduce_wait   :  1;
        bool auto_up_PFC   :  1;
        bool startup_test  :  1;
        bool F1_yes_no     :  1;
        bool roR_10_12     :  1;
        u32 reserved       : 18;
        array_union(u32, clock_max,
            u32 PLL, IFC, SFC, BFC, PFC;
        );
    );
    array_union(u32, mem_data,
        u32 roR[10], raR[8];
        union {
            u32 raW[7];
            u32 TRC[4];
        };
    );
    u32 *ROM_read_addr;
    u32 *SRAM_read_addr;
    array_union(struct cpg_overclock_setting, presets,
        struct cpg_overclock_setting F2, F3, F4, F5;
    );
} Xtune_config_t;

extern Xtune_config_t config;

void init_mem_data();
void init_config();
bool load_config();
bool save_config();
