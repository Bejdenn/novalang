#ifndef NODE_H
#define NODE_H

#include "symbol/symbol.h"
#include <stdlib.h>
#include <stdio.h>

extern int yylineno;

enum nodetype
{
    STATEMENT = 0,
    DECLARATION,
    ASSIGNMENT,
    REFERENCE,
    BUILTIN,
    IF_STMT,
    IF_EXPR,
    FOR_STMT,
    ARG_LIST,
    BLOCK,
    USER_FUNCTION,
    FN_BLOCK,
    INDEX,
    INDEX_ASSIGNMENT,
    CAST
};

struct ast
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *l;
    struct ast *r;
};

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r);

extern char *errors[];
extern int error_count;

void emit_error(char *s, ...);

void type_error(enum value_type expected, enum value_type got);

#endif // NODE_H