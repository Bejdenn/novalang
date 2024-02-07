#ifndef BLOCK_H
#define BLOCK_H

#include "node.h"

struct block
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol_table_entry *scope;
    struct ast *stmts;
    struct ast *expr;
};

struct ast *ast_newnode_block(struct ast *stmts, struct ast *expr, struct symbol_table_entry *scope, enum scope_type scope_to_exit);

#endif // BLOCK_H