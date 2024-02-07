#ifndef OP_H
#define OP_H

#include "node.h"

enum op
{
    ADD = 17,
    MINUS,
    MUL,
    DIV,
    GRT,
    MOD,
    EQ,
    LESS,
    GRT_OR_EQ,
    LESS_OR_EQ,
    N_EQ
};

union s_val *add(union s_val *l, union s_val *r);

union s_val *concat(union s_val *l, union s_val *r);

union s_val *sub(union s_val *l, union s_val *r);

union s_val *mul(union s_val *l, union s_val *r);

union s_val *divison(union s_val *l, union s_val *r);

union s_val *mod(union s_val *l, union s_val *r);

union s_val *eq_num(union s_val *l, union s_val *r);

union s_val *eq_str(union s_val *l, union s_val *r);

union s_val *eq_bool(union s_val *l, union s_val *r);

union s_val *neq_num(union s_val *l, union s_val *r);

union s_val *neq_str(union s_val *l, union s_val *r);

union s_val *neq_bool(union s_val *l, union s_val *r);

union s_val *grt(union s_val *l, union s_val *r);

union s_val *less(union s_val *l, union s_val *r);

union s_val *grt_or_eq(union s_val *l, union s_val *r);

union s_val *less_or_eq(union s_val *l, union s_val *r);

typedef union s_val *(*binary_op)(union s_val *, union s_val *);

struct opnode
{
    int nodetype;
    int lineno;
    enum value_type type;
    binary_op fn;
    struct ast *l;
    struct ast *r;
};

struct ast *ast_newnode_op(enum op op, struct ast *l, struct ast *r);

#endif // OP_H