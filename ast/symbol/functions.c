#include "functions.h"
#include <string.h>
#include <stdlib.h>

struct fn_symbol *fn_table;

struct fn_symbol *fn_get(char *s, enum fn_ns ns)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (fn_table[i].name && strcmp(fn_table[i].name, s) == 0 && fn_table[i].ns == ns)
            return &fn_table[i];
    }

    return NULL;
}

void fn_table_init()
{
    fn_table = malloc(sizeof(struct fn_symbol) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        fn_table[i].name = NULL;
    }

    struct symbol *s1, *s2;

    struct symbol **p = malloc(sizeof(struct symbol) * 1);

    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_STR;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print", NS_STR, T_VOID, p, 1);

    p = malloc(sizeof(struct symbol) * 1);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_STR;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("upcase", NS_STR, T_STR, p, 1);

    p = malloc(sizeof(struct symbol) * 1);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_STR;
    s1->val = malloc(sizeof(union s_val));

    s2 = malloc(sizeof(struct symbol) * 1);
    s2->type = T_STR;
    s2->val = malloc(sizeof(union s_val));

    p[0] = s1;
    p[1] = s2;
    fn_add("split", NS_STR, T_STR | T_ARRAY, p, 2);

    p = malloc(sizeof(struct symbol) * 1);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_STR | T_ARRAY;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print", NS_STR | NS_ARRAY, T_VOID, p, 1);

    p = malloc(sizeof(struct symbol) * 1);

    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print", NS_INT, T_VOID, p, 1);

    fn_add("read", NS_INT, T_INT, NULL, 0);

    p = malloc(sizeof(struct symbol) * 2);

    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT;
    s1->val = malloc(sizeof(union s_val));

    s2 = malloc(sizeof(struct symbol) * 1);
    s2->type = T_INT;
    s2->val = malloc(sizeof(union s_val));

    p[0] = s1;
    p[1] = s2;
    fn_add("random", NS_INT, T_INT, p, 2);

    p = malloc(sizeof(struct symbol) * 1);

    fn_add("read", NS_INT | NS_ARRAY, T_INT | T_ARRAY, NULL, 0);

    p = malloc(sizeof(struct symbol) * 2);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT | T_ARRAY;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print", NS_INT | NS_ARRAY, T_VOID, p, 1);

    p = malloc(sizeof(struct symbol) * 1);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT | T_ARRAY;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("len", NS_INT | NS_ARRAY, T_INT, p, 1);
}

struct fn_symbol *fn_add(char *name, enum fn_ns ns, enum value_type return_type, struct symbol *params[], int params_count)
{
    struct fn_symbol *fn = fn_get(name, ns);
    if (fn)
    {
        // As function declarations are only permitted in the global scope, the only thing to look out for are
        // duplicate declarations
        return NULL;
    }

    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (!fn_table[i].name)
        {
            fn_table[i].name = strdup(name);
            fn_table[i].ns = ns;
            fn_table[i].return_type = return_type;

            if (params)
            {
                fn_table[i].params = malloc(sizeof(struct symbol) * params_count);
                for (int j = 0; j < params_count; j++)
                {
                    fn_table[i].params[j] = params[j];
                }
            }

            fn_table[i].params_count = params_count;
            return &fn_table[i];
        }
    }

    return NULL;
}