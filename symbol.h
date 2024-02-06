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
    S_GLOBAL_SCOPE = 1 << 1,
    S_FUNCTION_SCOPE = 1 << 2,
    S_BLOCK_SCOPE = 1 << 3,
    S_LOCAL_SCOPE = 1 << 4
};

struct symbol
{
    int level;
    int index;
    enum scope_type scope;
    enum value_type type;
    union s_val *val;
};

typedef struct
{
    struct symbol **val;
    int size;
} stack;

stack *stack_new(void);

int stack_is_empty(const stack *s);

struct symbol *stack_peek(const stack *s);

char *lookup_value_type_name(int type);

struct array
{
    int size;
    union s_val *items;
};

union s_val
{
    int num;
    char *str;
    int boolean;
    struct array *array;
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
    int index;
};
enum symbol_err
{
    SYMBOL_ALREADY_DEFINED,
};

int symbol_is_visible(struct symbol *s);

struct symbol *symbol_get(char *name);

struct symbol *symbol_add(char *name, struct symbol *s, enum symbol_err *errors);

struct symbol_table_entry *scope_start(enum scope_type scope);

struct symbol_table_entry *scope_end(enum scope_type scope, struct symbol_table_entry *previous_table);

extern struct fn_symbol *fn_table;

void fn_table_init();

struct fn_symbol *fn_get(char *name);

struct fn_symbol *fn_add(char *name, enum value_type return_type, struct symbol **params, int params_count);

#endif // SYMBOL_H
