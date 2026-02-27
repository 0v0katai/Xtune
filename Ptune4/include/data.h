#include <gint/clock.h>
#include <gint/defs/types.h>

typedef struct
{
    lword_union(settings_data,
        u8 ROM_margin      :  4;
        u8 RAM_margin      :  4;
        bool unlocked_mode :  1;
        bool reduce_wait   :  1;
        bool auto_up_PFC   :  1;
        bool startup_test  :  1;
        u32 reserved       : 16;
        u8 platform        :  4;
    );
    u32 div_max_freq[5];
    u32 reserved[2];
    struct cpg_overclock_setting preset_save[4];
    u32 ROM_read_data[10];
    u32 RAM_read_data[8];
    u32 RAM_write_data[7];
} Xtune_config_t;

bool load_config();
bool save_config();