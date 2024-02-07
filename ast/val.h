#ifndef VAL_H
#define VAL_H

#include "node.h"
#include "symbol/symbol.h"

struct strval
{
    int nodetype;
    int lineno;
    enum value_type type;
    char *str;
};

struct numval
{
    int nodetype;
    int lineno;
    enum value_type type;
    int number;
};

struct boolval
{
    int nodetype;
    int lineno;
    enum value_type type;
    int boolean;
};

struct arrayval
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *items;
};

struct cast
{
    int nodetype;
    int lineno;
    enum value_type cast_to;
    struct ast *expr;
};

struct ast *ast_newnode_str(char *s);

struct ast *ast_newnode_num(int d);

struct ast *ast_newnode_bool(int b);

struct ast *ast_newnode_array(struct ast *items);

struct ast *ast_newnode_cast(enum value_type cast_to, struct ast *expr);

union s_val *cast(enum value_type dest, enum value_type src, union s_val *v);

void copy_array(union s_val *dest, union s_val *src);

#endif // VAL_H