#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct symbol symtab[TBL_SIZE];
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

struct symbol *symlookup(char *s)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (symtab[i].name && strcmp(symtab[i].name, s) == 0)
            return &symtab[i];
    }

    return NULL;
}

struct symbol *symadd(char *s, int type)
{
    struct symbol *sym = symlookup(s);
    if (sym)
    {
        // if the symbol already exists, we replace its type to allow further type checks to run
        sym->type = type;
        return sym;
    }

    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (!symtab[i].name)
        {
            symtab[i].name = strdup(s);
            symtab[i].type = type;
            symtab[i].val = malloc(sizeof(union s_val));
            return &symtab[i];
        }
    }

    /* at this point we don't have any space in the table anymore */

    return NULL;
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
