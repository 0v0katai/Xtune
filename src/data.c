#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "util.h"
#include "config.h"

static u32 get_file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    fseek(f, 0, SEEK_SET);
    return size;
}

static void set_header() {
    config.hash = XTUNE_HASH;
    #if CP400
        memcpy(config.model, (char *)0x80000302, sizeof(char[3]));
    #else
        memcpy(config.model, (char *)0x80000300, sizeof(char[4]));
    #endif
}

struct cpg_overclock_setting get_gint_preset(int i) {
    struct cpg_overclock_setting cur, ret;
    cpg_get_overclock_setting(&cur);
    clock_set_speed(i);
    cpg_get_overclock_setting(&ret);
    cpg_set_overclock_setting(&cur);
    return ret;
};

void init_mem_data() {
    memcpy(config.roR, roR_defs, sizeof(config.roR));
    config.ROM_read_addr = NULL;
    #if !CG50 && !CG100 && !CP400
        memcpy(config.raR, raR_defs, sizeof(config.raR));
        memcpy(config.raW, raW_defs, sizeof(config.raW));
        config.SRAM_read_addr = NULL;
    #else
        memcpy(config.TRC, TRC_defs, sizeof(config.TRC));
    #endif
}

void init_config() {
    memset(&config, 0, sizeof(Xtune_config_t));
    set_header();
    memcpy(config.setup, setup_defs, sizeof(config.setup));
    init_mem_data();

    for (int i = CLOCK_SPEED_F2; i <= CLOCK_SPEED_F5; i++)
        config.presets[i - CLOCK_SPEED_F2] = get_gint_preset(i);
}

bool load_config() {
    FILE *f = fopen("xtune.sav", "rb");
    if (f == NULL)
        return false;
    if (get_file_size(f) != sizeof(Xtune_config_t)) {
        fclose(f);
        return false;
    }

    fread(&config, sizeof(Xtune_config_t), 1, f);
    fclose(f);

    set_header();
    return true;
}

bool save_config()
{
    FILE *f = fopen("xtune.sav", "wb");
    if (f == NULL)
        return false;

    const size_t written = fwrite(&config, sizeof(Xtune_config_t), 1, f);
    fclose(f);

    return written;
}
