#ifndef REF_H
#define REF_H

#include "node.h"

struct symref
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
};

struct symindex
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *index;
};

struct ast *ast_newnode_ref(char *sym_name);

struct ast *ast_newnode_index(char *sym_name, struct ast *index);

#endif // REF_H