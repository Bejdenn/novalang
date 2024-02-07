#ifndef FLOW_H
#define FLOW_H

#include "node.h"

struct flow
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *condition;
    struct ast *block;
    struct ast *branches;
};

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *block, struct ast *branches);

struct ast *ast_newnode_pipe(struct ast *l, struct ast *r);

#endif