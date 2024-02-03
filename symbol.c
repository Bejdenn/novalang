#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct symbol *symbol_table;
int level = 0;

struct fn_symbol BUILTIN_FUNCTIONS[TBL_SIZE] =
    {{.name = "print", .return_type = T_VOID, .params_count = 1, .params = (struct symbol[]){{.name = "s", .type = T_STR}}},
     {.name = "print_int", .return_type = T_VOID, .params_count = 1, .params = (struct symbol[]){{.name = "i", .type = T_INT}}},
     {.name = "read_int", .return_type = T_INT, .params = 0, .params_count = 0},
     {.name = "random_int", .return_type = T_INT, .params_count = 2, .params = (struct symbol[]){{.name = "lower", .type = T_INT}, {.name = "upper", .type = T_INT}}}};

char *lookup_value_type_name(enum value_type type)
{
    switch (type)
    {
    case T_INT:
        return "int";
    case T_FLT:
        return "float";
    case T_STR:
        return "string";
    case T_BOOL:
        return "bool";
    case T_UNKNOWN:
        return "unknown";
    case T_VOID:
        return "void";
    default:
        return "never";
    }
}

struct symbol *symbol_get(char *name)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (symbol_table[i].name && strcmp(symbol_table[i].name, name) == 0)
            return &symbol_table[i];
    }

    return NULL;
}

void symbol_table_init()
{
    symbol_table = malloc(sizeof(struct symbol) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        symbol_table[i].name = NULL;
        symbol_table[i].level = level;
    }
}

struct symbol *symbol_add(char *name, int type, union s_val *val)
{
    struct symbol *sym = symbol_get(name);
    if (sym)
    {
        // replace the symbol
        sym->type = type;
        sym->val = val;
        sym->level = level;
        return sym;
    }

    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (!symbol_table[i].name)
        {
            symbol_table[i].name = strdup(name);
            symbol_table[i].type = type;
            symbol_table[i].level = level;

            if (val)
            {
                *symbol_table[i].val = *val;
            }
            else
            {
                symbol_table[i].val = malloc(sizeof(union s_val));
            }
            return &symbol_table[i];
        }
    }

    /* at this point we don't have any space in the table anymore */

    return NULL;
}

struct symbol *symbol_table_copy()
{
    struct symbol *new_t = malloc(sizeof(struct symbol) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        new_t[i] = symbol_table[i];
        if (symbol_table[i].name)
        {
            new_t[i].name = strdup(symbol_table[i].name);
            new_t[i].level = level;
            new_t[i].val = malloc(sizeof(union s_val));
            *new_t[i].val = *symbol_table[i].val;
            new_t[i].type = symbol_table[i].type;
        }
    }
    return new_t;
}

struct symbol *nested_scope_start(struct symbol *new_t)
{
    struct symbol *old_t = symbol_table;
    symbol_table = symbol_table_copy();
    // the symbol table has to contain all symbols from the inner scope that are not shadowed
    for (int i = 0; new_t && i < TBL_SIZE; i++)
    {
        if (new_t[i].name && symbol_get(new_t[i].name) == NULL)
        {
            symbol_add(new_t[i].name, new_t[i].type, NULL);
        }
    }

    level++;
    return old_t;
}

struct symbol *nested_scope_end(struct symbol *old_t)
{
    struct symbol *current_t = symbol_table;
    // merge changes from the nested scope on not shadowed symbols
    for (int i = 0; i < TBL_SIZE; i++)
    {
        for (int j = 0; j < TBL_SIZE; j++)
        {
            if (old_t[i].name && symbol_table[j].name && strcmp(old_t[i].name, symbol_table[j].name) == 0 && old_t[i].level == symbol_table[j].level)
            {
                old_t[i] = symbol_table[j];
            }
        }
    }

    level--;
    symbol_table = old_t;
    return current_t;
}

struct fn_symbol *fnlookup(char *s)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (BUILTIN_FUNCTIONS[i].name && strcmp(BUILTIN_FUNCTIONS[i].name, s) == 0)
            return &BUILTIN_FUNCTIONS[i];
    }

    return NULL;
}
