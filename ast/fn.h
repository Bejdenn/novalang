#ifndef FN_H
#define FN_H

#include "node.h"
#include "symbol/functions.h"
#include "declaration.h"

struct routine
{
    struct fn_symbol *fn;
    struct ast *block;
};

extern struct routine routines[100];

struct routine *routine_get(struct fn_symbol *fn);

struct fn_call
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct fn_symbol *fn;
    struct ast *args;
    int args_count;
};

struct fn_symbol *function_signature(char *fn_name, struct ast *params, enum value_type type);

struct ast *ast_newnode_builtin_fn_call(char *fn, enum fn_ns ns, struct ast *args);

struct ast *ast_newnode_fn_call(char *fn, enum fn_ns ns, struct ast *args);

struct ast *ast_newnode_fn_decl(struct fn_symbol *fn_sign, struct ast *block);

union s_val *call_builtin_fn(struct fn_symbol *fn, struct ast *args);

#endif // FN_H