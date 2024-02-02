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

#define TBL_SIZE 25

extern struct symbol symtab[TBL_SIZE];
extern struct fn_symbol BUILTIN_FUNCTIONS[TBL_SIZE];

struct symbol *symlookup(char *);
struct symbol *symadd(char *, int);
struct fn_symbol *fnlookup(char *);
