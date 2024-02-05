#ifndef SYMBOL_H
#define SYMBOL_H

#define TBL_SIZE 99

enum value_type
{
    T_INT = 18,
    T_FLT,
    T_STR,
    T_BOOL,
    T_UNKNOWN,
    T_VOID
};

enum scope_type
{
    S_GLOBAL_SCOPE,
    S_FUNCTION_SCOPE,
    S_BLOCK_SCOPE,
    S_LOCAL_SCOPE
};

struct symbol
{
    int level;
    enum scope_type scope;
    enum value_type type;
    union s_val *val;
};

typedef struct
{
    struct symbol **val;
    int size;
} stack;

char *lookup_value_type_name(enum value_type type);

union s_val
{
    int num;
    char *str;
    int boolean;
};

struct fn_symbol
{
    char *name;
    enum value_type return_type;
    struct symbol **params;
    int params_count;
};

struct symbol_table_entry
{
    char *name;
    stack *references;
};

void symbol_table_init();

extern struct symbol_table_entry *symbol_table;

struct context
{
    int level;
    enum scope_type scope;
};

struct symbol *symbol_get(char *name);

struct symbol *symbol_add(char *name, struct symbol *s);

struct symbol_table_entry *scope_start(enum scope_type scope);

void scope_end(struct symbol_table_entry *previous_table);

extern struct fn_symbol *fn_table;

void fn_table_init();

struct fn_symbol *fn_get(char *name);

struct fn_symbol *fn_add(char *name, enum value_type return_type, struct symbol **params, int params_count);

#endif // SYMBOL_H
