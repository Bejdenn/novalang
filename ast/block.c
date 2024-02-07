#include "block.h"

struct ast *ast_newnode_block(struct ast *stmts, struct ast *expr, struct symbol_table_entry *prev_scope, enum scope_type scope_to_exit)
{
    struct block *a = malloc(sizeof(struct block));
    a->nodetype = BLOCK;

    a->type = T_VOID;
    if (expr != NULL)
    {
        a->type = expr->type;
    }

    a->lineno = yylineno;
    a->stmts = stmts;
    a->expr = expr;

    a->scope = scope_end(scope_to_exit, prev_scope);

    return (struct ast *)a;
}
