#include <gint/display.h>
#include <stdbool.h>
#include <stddef.h>

#include "help.h"
#include "util.h"
#include "config.h"

static help_function_t help_function = NULL;

#if HELP
static void help_info_express() {
#if CP400
    info_box(12, 1, C_BLACK, "HELP",
        "[KBD]: Reset to default\n"
        "[DEL]: Toggle benchmark\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: -/+ option value\n"
        "[/][x]: -/+ roR\n"
        "[-][+]: -/+ TRC\n"
        "[SHIFT][-][+]: -/+ CL\n"
        "[=]: Settings\n"
        "[x]: BSC settings\n"
        "[y]: Memory data & tests\n"
        "[Clear]: Close help / Quit CPtune4");
#elif CG100
    info_box(0, 1, C_BLACK, "HELP",
        "[ON]: Reset to default\n"
        "[|<-][->|]: Select preset, [OK]: Confirm\n"
        "[PGUP]: Toggle benchmark\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: -/+ option value\n"
        "[x][/]: +/- roR\n"
        "[+][-]: +/- TRC\n"
        "[SHIFT][+][-]: +/- CL\n"
        "[SETTINGS]: Settings\n"
        "[VARIABLE]: BSC settings\n"
        "[TOOLS]: Memory data & tests\n"
        "[SHIFT][HOME]: Quit Ptune4\n"
        "[BACK]: Close help");
#else
    info_box(0, 1, C_BLACK, "HELP",
        "[F1]-[F5]: Apply preset\n"
        "[F6]: Toggle benchmark\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: -/+ option value\n"
        "[x][/]: +/- roR\n"
    #if CG50
        "[+][-]: +/- TRC\n"
        "[SHIFT][+][-]: +/- CL\n"
    #else
        "[+]/[-]: +/- raR\n"
        "[SHIFT][+]/[-]: +/- raW\n"
    #endif
        "[OPTN]: Memory data & tests\n"
        "[VARS]: BSC settings\n"
        "[SHIFT][MENU]: Settings\n"
        "[MENU]: Return to OS\n"
        "[EXIT]: Close help");
#endif
}

static void help_info_mem_data() {
#if CP400
    info_box(15, 1, C_BLACK, "HELP",
        "[=]: Reset to default\n"
        "[x]: Toggle margin view\n"
        "[y]: ROM read test\n"
        "[z]: SDRAM write test\n"
        "[Clear]: Close help / < Express menu");
#elif CG100
    info_box(4, 1, C_BLACK, "HELP",
        "[ON]: Reset to default\n"
        "[|<-]: Toggle margin view\n"
        "[->|]: ROM read test\n"
        "[PGUP]: SDRAM write test\n"
        "[BACK]: Close help / < Express menu");
#elif CG50
    info_box(4, 1, C_BLACK, "HELP",
        "[F1]: Reset to default\n"
        "[F3]: Toggle margin view\n"
        "[F5]: ROM read test\n"
        "[F6]: SDRAM write test\n"
        "[EXIT]: Close help / < Express menu");
#else
    info_box(3, 1, C_BLACK, "HELP",
        "[F1]: Reset to default\n"
        "[F2]: Toggle SRAM read/write view\n"
        "[F3]: Toggle margin view\n"
        "[F5]: ROM read test\n"
        "[F6]: SRAM read/write test\n"
        "[EXIT]: Close help / < Express menu");
#endif
}

static void help_info_bsc() {
#if CP400
    info_box(15, 1, C_BLACK, "HELP",
        "[-][+]: -/+ option value\n"
        "[x]: Toggle BCR/WCR mode\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: Select CSn area\n"
        "[Clear]: Close help / < Express menu");
#elif CG100
    info_box(3, 1, C_BLACK, "HELP",
        "[+]: Increase option value\n"
        "[-]: Decrease option value\n"
        "[|<-][->|]: Toggle BCR/WCR mode\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: Select CSn area\n"
        "[PGUP][PGDW]: Quick jump to previous/next row\n"
        "[BACK]: Close help / < Express menu");
#else
    info_box(3, 1, C_BLACK, "HELP",
        "[F1][+]: Increase option value\n"
        "[F2][-]: Decrease option value\n"
        "[F6]: Toggle BCR/WCR mode\n"
        "[UP][DOWN]: Select option\n"
        "[LEFT][RIGHT]: Select CSn area\n"
        "[EXIT]: Close help / < Express menu");
#endif
}

void help_info_setup() {
#if CP400
    info_box(15, 1, C_BLACK, "HELP",
        "[=]: Reset to default\n"
        "[-][+]: -/+ option value (1 MHz)\n"
        "[LEFT][RIGHT]: -/+ option value (0.1 MHz)\n"
        "[UP][DOWN]: Select option\n"
        "[/]: About this add-in\n"
        "[Clear]: Close help / < Express menu");
#elif CG100
    info_box(4, 1, C_BLACK, "HELP",
        "[ON]: Reset to default\n"
        "[+][-]: +/- option value (1 MHz)\n"
        "[LEFT][RIGHT]: -/+ option value (0.1 MHz)\n"
        "[UP][DOWN]: Select option\n"
        "[PGUP]: About this add-in\n"
        "[BACK]: Close help / < Express menu");
#else
    info_box(3, 1, C_BLACK, "HELP",
        "[F1]: Reset to default\n"
        "[F2][+]: Increase option value (1 MHz)\n"
        "[F3][-]: Decrease option value (0.1 MHz)\n"
        "[LEFT][RIGHT]: -/+ option value (0.1 MHz)\n"
        "[UP][DOWN]: Select option\n"
        "[F6]: About this add-in\n"
        "[EXIT]: Close help / < Express menu");
#endif
}
#endif

void set_help_function(const int menu) {
#if HELP
    if (menu == HELP_EXPRESS)
        help_function = help_info_express;
    else if (menu == HELP_MEM_DATA)
        help_function = help_info_mem_data;
    else if (menu == HELP_BSC)
        help_function = help_info_bsc;
    else if (menu == HELP_SETUP)
        help_function = help_info_setup;
#else
    (void)menu;
#endif
}

void call_help_function() {
#if HELP
    extern bool help_status;
    global.shift = false;
    if (help_status)
        return;
    help_status = true;
    help_function();
    while (xtune_getkey().key != KEY_EXIT) {}
    help_status = false;
#endif
}