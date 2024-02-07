#include "flow.h"

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *block, struct ast *branches)
{
    struct flow *a = malloc(sizeof(struct flow));
    a->nodetype = nodetype;
    a->lineno = yylineno;
    if (condition != NULL && condition->type != T_BOOL)
    {
        emit_error("Condition must be a boolean, but was '%s'", lookup_value_type_name(condition->type));
    }
    a->condition = condition;

    if (branches != NULL && block->type != branches->type)
    {
        emit_error("Branches must be of the same type ('%s' vs. '%s')", lookup_value_type_name(block->type),
                   lookup_value_type_name(branches->type));
    }

    a->type = block->type;
    a->block = block;
    a->branches = branches;
    return (struct ast *)a;
}

struct ast *ast_newnode_pipe(struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));
    a->nodetype = STATEMENT; // Statement because we just have to evaluate a tree of statements and expressions
    a->lineno = yylineno;
    a->l = l;
    a->r = r;

    a->type = r->type;
    return a;
}