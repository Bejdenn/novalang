#ifndef ASSIGNMENT_H
#define ASSIGNMENT_H

#include "node.h"

struct symassign
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *v;
};

struct indexassign
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *index;
    struct ast *v;
};

struct ast *ast_newnode_assign(char *sym_name, struct ast *v);

struct ast *ast_newnode_index_assign(char *sym_name, struct ast *index, struct ast *v);

#endif // ASSIGNMENT_H