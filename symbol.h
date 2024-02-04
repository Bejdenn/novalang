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

enum symbol_type
{
    S_VARIABLE,
    S_FUNCTION
};

struct symbol
{
    char *name;
    enum symbol_type s_type;
    union s_val *val;
    enum value_type type;
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
    enum symbol_type s_type;
    enum value_type return_type;
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
struct symbol *symbol_add(char *, enum value_type, union s_val *);

struct symbol *nested_scope_start(struct symbol *);
struct symbol *nested_scope_end(struct symbol *);

extern struct fn_symbol *fn_table;

void fn_table_init();
struct fn_symbol *fn_get(char *);
struct fn_symbol *fn_add(char *, enum value_type, struct symbol *, int);
