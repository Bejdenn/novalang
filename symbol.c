#include "symbol.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

stack *stack_new(void)
{
    return calloc(1, sizeof(stack));
}

stack *stack_copy(const stack *s)
{
    stack *new_s = stack_new();
    new_s->val = malloc(s->size * sizeof(*s->val));
    new_s->size = s->size;
    for (int i = 0; i < s->size; i++)
    {
        new_s->val[i] = s->val[i];
    }
    return new_s;
}

void stack_push(stack *s, struct symbol *elem)
{
    s->val = realloc(s->val, (s->size + 1) * sizeof elem);
    s->val[s->size++] = elem;
}

struct symbol *stack_pop(stack *s)
{
    return s->val[--s->size];
}

int stack_is_empty(const stack *s)
{
    return s->size == 0;
}

struct symbol *stack_peek(const stack *s)
{
    if (stack_is_empty(s))
        return NULL;
    return s->val[s->size - 1];
}

struct symbol_table_entry *symbol_table;
struct context context = {.level = 0, .scope = S_GLOBAL_SCOPE, .index = 0};

struct fn_symbol *fn_table;

// TODO rewrite using a "map" (access static list by index)
char *lookup_value_type_name(int type)
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
    case T_INT | T_ARRAY:
        return "int[]";
    case T_FLT | T_ARRAY:
        return "float[]";
    case T_STR | T_ARRAY:
        return "string[]";
    case T_BOOL | T_ARRAY:
        return "bool[]";
    default:
        return "never";
    }
}

void symbol_table_init()
{
    symbol_table = malloc(sizeof(struct symbol_table_entry) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        symbol_table[i].name = NULL;
        symbol_table[i].references = stack_new();
    }
}

int symbol_is_visible(struct symbol *s)
{
    return s->level <= context.level || s->index == context.index;
}

struct symbol *symbol_get(char *name)
{
    for (int i = 0; i < TBL_SIZE; i++)
        if (symbol_table[i].name && strcmp(symbol_table[i].name, name) == 0)
            return stack_peek(symbol_table[i].references);

    return NULL;
}

struct symbol *symbol_add(char *name, struct symbol *s)
{
    s->level = context.level;
    s->scope = context.scope;

    struct symbol *sym = symbol_get(name);
    if (sym)
    {
        int local = (s->scope & S_LOCAL_SCOPE) == S_LOCAL_SCOPE && (context.scope & S_LOCAL_SCOPE) == S_LOCAL_SCOPE;
        int can_be_shadowed = (s->scope & S_GLOBAL_SCOPE) == S_GLOBAL_SCOPE && (context.scope & S_FUNCTION_SCOPE) == S_FUNCTION_SCOPE;

        if (!local && !can_be_shadowed && symbol_is_visible(sym) && symbol_is_visible(s))
        {
            fprintf(stderr, "redeclaration of '%s'", name);
            exit(1);
        }
        else
        {
            for (int i = 0; i < TBL_SIZE; i++)
            {
                if (symbol_table[i].name && (strcmp(symbol_table[i].name, name) == 0))
                {
                    stack_push(symbol_table[i].references, s);
                    return symbol_get(name);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < TBL_SIZE; i++)
        {
            if (!symbol_table[i].name)
            {
                symbol_table[i].name = strdup(name);
                stack_push(symbol_table[i].references, s);
                return symbol_get(name);
            }
        }
    }

    /* at this point we don't have any space in the table anymore */

    return NULL;
}

struct symbol_table_entry *symbol_table_copy()
{
    struct symbol_table_entry *new_t = malloc(sizeof(struct symbol_table_entry) * TBL_SIZE);
    for (int i = 0; i < TBL_SIZE; i++)
    {
        new_t[i].name = symbol_table[i].name;
        new_t[i].references = stack_copy(symbol_table[i].references);
    }
    return new_t;
}

struct symbol_table_entry *scope_start(enum scope_type scope)
{
    struct symbol_table_entry *current_table = symbol_table;
    if (scope == S_FUNCTION_SCOPE)
    {
        symbol_table = symbol_table_copy();
    }
    context.level++;
    context.index++;

    context.scope |= scope;

    return current_table;
}

struct symbol_table_entry *scope_end(enum scope_type type, struct symbol_table_entry *previous_table)
{
    context.level--;
    struct symbol_table_entry *current_table = symbol_table;
    if (type == S_FUNCTION_SCOPE)
    {
        symbol_table = previous_table;
    }
    context.scope ^= type;
    return current_table;
}

struct fn_symbol *fn_get(char *s)
{
    for (int i = 0; i < TBL_SIZE; i++)
    {
        if (fn_table[i].name && strcmp(fn_table[i].name, s) == 0)
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
    fn_add("print", T_VOID, p, 1);

    p = malloc(sizeof(struct symbol) * 1);

    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print_int", T_VOID, p, 1);

    fn_add("read_int", T_INT, NULL, 0);

    p = malloc(sizeof(struct symbol) * 2);

    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT;
    s1->val = malloc(sizeof(union s_val));

    s2 = malloc(sizeof(struct symbol) * 1);
    s2->type = T_INT;
    s2->val = malloc(sizeof(union s_val));

    p[0] = s1;
    p[1] = s2;
    fn_add("random_int", T_INT, p, 2);

    p = malloc(sizeof(struct symbol) * 1);

    fn_add("read_ints", T_INT | T_ARRAY, NULL, 0);

    p = malloc(sizeof(struct symbol) * 2);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT | T_ARRAY;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("print_arr", T_VOID, p, 1);

    p = malloc(sizeof(struct symbol) * 1);
    s1 = malloc(sizeof(struct symbol) * 1);
    s1->type = T_INT | T_ARRAY;
    s1->val = malloc(sizeof(union s_val));

    p[0] = s1;
    fn_add("len", T_INT, p, 1);
}

struct fn_symbol *fn_add(char *name, enum value_type return_type, struct symbol *params[], int params_count)
{
    struct fn_symbol *fn = fn_get(name);
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
