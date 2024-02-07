#ifndef AST_H
#define AST_H

#include "symbol/symbol.h"
#include "symbol/functions.h"

#include "node.h"
#include "declaration.h"
#include "val.h"
#include "op.h"
#include "assignment.h"
#include "fn.h"
#include "ref.h"
#include "flow.h"
#include "block.h"

struct arglist
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *expr;
    struct arglist *next;
};

void ast_interpret(struct ast *);

union s_val *ast_eval(struct ast *);

void ast_free(struct ast *);

#endif // AST_H