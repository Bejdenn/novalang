#include "symbol.h"
#include <string.h>
#include <stdlib.h>

struct symbol *symlookup(char *s)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (symtab[i].name && strcmp(symtab[i].name, s) == 1)
            return &symtab[i];

        if (!symtab[i].name)
        {
            symtab[i].name = strdup(s);
            return &symtab[i];
        }
    }

    /* at this point we don't have any space in the table anymore */

    return 0;
}
