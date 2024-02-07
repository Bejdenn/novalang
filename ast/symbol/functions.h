#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "symbol.h"

enum fn_ns
{
    NS_STR = T_STR,
    NS_INT = T_INT,
    NS_BOOL = T_BOOL,
    NS_ARRAY = T_ARRAY,
    NS_GLOBAL = T_ARRAY << 1,
};

struct fn_symbol
{
    char *name;
    enum fn_ns ns;
    enum value_type return_type;
    struct symbol **params;
    int params_count;
};

extern struct fn_symbol *fn_table;

void fn_table_init();

struct fn_symbol *fn_get(char *name, enum fn_ns ns);

struct fn_symbol *fn_add(char *name, enum fn_ns ns, enum value_type return_type, struct symbol **params, int params_count);

#endif // FN_H