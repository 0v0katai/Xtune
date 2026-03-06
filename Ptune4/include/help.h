#ifndef XTUNE_HELP_H
#define XTUNE_HELP_H

enum help {
    HELP_EXPRESS,
    HELP_MEM_DATA,
    HELP_BSC,
    HELP_SETUP
};

typedef void (*help_function_t)(void);

void call_help_function();

void set_help_function(int menu);

#endif // XTUNE_HELP_H