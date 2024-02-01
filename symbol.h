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

struct value
{
    int type;
    union s_val *u;
};

union s_val
{
    int num;
    char *str;
    int boolean;
};

#define TBL_SIZE 25
extern struct symbol symtab[TBL_SIZE];

struct symbol *symlookup(char *);
struct symbol *symadd(char *, int);
