#ifndef DECLARATION_H
#define DECLARATION_H

#include "node.h"
#include "symbol/symbol.h"

struct symdecl
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
};

struct ast *ast_newnode_decl(char *sym_name, enum value_type type);

#endif // DECLARATION_H