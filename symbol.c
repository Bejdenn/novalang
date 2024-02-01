#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct symbol symtab[TBL_SIZE];

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

    return 0;
}

struct symbol *symadd(char *s, int type)
{
    struct symbol *sym = symlookup(s);
    if (sym)
    {
        // TODO: when the lookup returns a symbol, we already
        // have a symbol with the same name in the table. Throw an error?
        return NULL;
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
