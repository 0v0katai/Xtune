#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "util.h"
#include "settings.h"

static u32 get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

bool load_config()
{   
    Xtune_config_t config;

    FILE *f = fopen("xtune.sav", "rb");
    if (f == NULL)
        return true;
    if (get_file_size(f) != sizeof(Xtune_config_t)) {
        fclose(f);
        return true;
    }

    fread(&config, sizeof(Xtune_config_t), 1, f);
    fclose(f);

    memcpy(&PLL_CLK_MAX, config.div_max_freq, sizeof(config.div_max_freq));
    ROM_MARGIN = config.settings_data.ROM_margin;
    RAM_MARGIN = config.settings_data.RAM_margin;
    UNLOCKED_MODE = config.settings_data.unlocked_mode;
    AUTO_REDUCE_WAIT = config.settings_data.reduce_wait;
    AUTO_UP_PFC = config.settings_data.auto_up_PFC;
    STARTUP_MEMORY_TEST = config.settings_data.startup_test;
    F1_YES_NO = config.settings_data.F1_yes_no;
    memcpy(&PRESET_F2, config.preset_save, sizeof(config.preset_save));
    memcpy(roR, config.ROM_read_data, sizeof(config.ROM_read_data));
    #if defined CG50 || defined CG100 || defined CP400
    memcpy(raW_TRC, config.RAM_write_data, sizeof(raW_TRC));
    #else
    memcpy(raR, config.RAM_read_data, sizeof(raR));
    memcpy(raW, config.RAM_write_data, sizeof(raW));
    #endif

    return false;
}

bool save_config()
{
    Xtune_config_t config = {0};
    memcpy(config.div_max_freq, &PLL_CLK_MAX, sizeof(config.div_max_freq));
    config.settings_data.lword = (ROM_MARGIN << 28)    | (RAM_MARGIN << 24) |
                                 (UNLOCKED_MODE << 23) | (AUTO_REDUCE_WAIT << 22) |
                                 (AUTO_UP_PFC << 21)   | (STARTUP_MEMORY_TEST << 20) |
                                 (F1_YES_NO << 19);
    config.settings_data.lword |= HARDWARE_TARGET;
    memcpy(config.preset_save, &PRESET_F2, sizeof(config.preset_save));
    memcpy(config.ROM_read_data, roR, sizeof(config.ROM_read_data));
    #if defined CG50 || defined CG100 || defined CP400
    memcpy(config.RAM_write_data, raW_TRC, sizeof(raW_TRC));
    #else
    memcpy(config.RAM_read_data, raR, sizeof(raR));
    memcpy(config.RAM_write_data, raW, sizeof(raW));
    #endif

    FILE *f = fopen("xtune.sav", "wb");
    if (f == NULL)
        return true;

    const size_t written = fwrite(&config, sizeof(Xtune_config_t), 1, f);
    fclose(f);

    return !written;
}
