#ifndef SYMBOL_H
#define SYMBOL_H

#define TBL_SIZE 99

enum value_type
{
    T_INT = 1 << 5,
    T_FLT = 1 << 6,
    T_STR = 1 << 7,
    T_BOOL = 1 << 8,
    T_UNKNOWN = 1 << 9,
    T_VOID = 1 << 10,
    T_ARRAY = 1 << 14
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

char *lookup_value_type_name(int type);

union s_val
{
    int num;
    char *str;
    int boolean;
    union s_val *array;
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
