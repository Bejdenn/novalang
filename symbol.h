enum value_type
{
    T_INT = 18,
    T_FLT,
    T_STR,
    T_BOOL, /* underscore to avoid collision with C bools*/
    T_UNKNOWN,
    T_VOID
};

char *lookup_value_type_name(enum value_type type);

struct symbol
{
    char *name;
    union s_val *val;
    int type;
    int level;
};

union s_val
{
    int num;
    char *str;
    int boolean;
};

struct fn_symbol
{
    char *name;
    int return_type;
    struct symbol *params;
    int params_count;
};

#define TBL_SIZE 99

void symbol_table_init();

/// @brief The symbol table (global scope).
extern struct symbol *symbol_table;

/// @brief The current level of nested scopes.
extern int level;

extern struct fn_symbol BUILTIN_FUNCTIONS[TBL_SIZE];

struct symbol *symbol_get(char *);
struct symbol *symbol_add(char *, int, union s_val *);

struct symbol *nested_scope_start(struct symbol *);
struct symbol *nested_scope_end(struct symbol *);

struct fn_symbol *fnlookup(char *);
