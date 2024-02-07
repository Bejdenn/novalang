#include "op.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *lookup_op(int op)
{
    switch (op)
    {
    case ADD:
        return "+";
    case MINUS:
        return "-";
    case MOD:
        return "%";
    case MUL:
        return "*";
    case DIV:
        return "/";
    case EQ:
        return "==";
    case N_EQ:
        return "!=";
    case GRT:
        return ">";
    case LESS:
        return "<";
    case GRT_OR_EQ:
        return ">=";
    case LESS_OR_EQ:
        return "<=";
    default:
        return "unknown";
    }
}

char *str_concat(char *s1, char *s2)
{
    char *s = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(s, s1);
    strcat(s, s2);
    return s;
}

union s_val *add(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->num = l->num + r->num;
    return v;
}

union s_val *concat(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->str = str_concat(l->str, r->str);
    return v;
}

union s_val *sub(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->num = l->num - r->num;
    return v;
}

union s_val *mul(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->num = l->num * r->num;
    return v;
}

union s_val *divison(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->num = l->num / r->num;
    return v;
}

union s_val *mod(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->num = l->num % r->num;
    return v;
}

union s_val *eq_num(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num == r->num;
    return v;
}

union s_val *eq_str(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = strcmp(l->str, r->str) == 0;
    return v;
}

union s_val *eq_bool(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->boolean == r->boolean;
    return v;
}

union s_val *neq_num(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num != r->num;
    return v;
}

union s_val *neq_str(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = strcmp(l->str, r->str) != 0;
    return v;
}

union s_val *neq_bool(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->boolean != r->boolean;
    return v;
}

union s_val *grt(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num > r->num;
    return v;
}

union s_val *less(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num < r->num;
    return v;
}

union s_val *grt_or_eq(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num >= r->num;
    return v;
}

union s_val *less_or_eq(union s_val *l, union s_val *r)
{
    union s_val *v = malloc(sizeof(union s_val));
    v->boolean = l->num <= r->num;
    return v;
}

binary_op get_op(int lineno, enum op op, struct ast *l, struct ast *r, enum value_type *type)
{
    switch (op)
    {
    case ADD:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_INT;
            return add;
        }
        else if (l->type == T_STR && r->type == T_STR)
        {
            *type = T_STR;
            return concat;
        }
        break;
    case MINUS:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_INT;
            return sub;
        }
        break;
    case MOD:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_INT;
            return mod;
        }
        break;
    case MUL:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_INT;
            return mul;
        }
        break;
    case DIV:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_INT;
            return divison;
        }
        break;
    case EQ:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return eq_num;
        }
        else if (l->type == T_STR && r->type == T_STR)
        {
            *type = T_BOOL;
            return eq_str;
        }
        else if (l->type == T_BOOL && r->type == T_BOOL)
        {
            *type = T_BOOL;
            return eq_bool;
        }
        break;
    case N_EQ:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return neq_num;
        }
        else if (l->type == T_STR && r->type == T_STR)
        {
            *type = T_BOOL;
            return neq_str;
        }
        else if (l->type == T_BOOL && r->type == T_BOOL)
        {
            *type = T_BOOL;
            return neq_bool;
        }
        break;
    case GRT:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return grt;
        }
        break;
    case LESS:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return less;
        }
        break;
    case GRT_OR_EQ:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return grt_or_eq;
        }
        break;
    case LESS_OR_EQ:
        if (l->type == T_INT && r->type == T_INT)
        {
            *type = T_BOOL;
            return less_or_eq;
        }
        break;
    default:
        fprintf(stderr, "%d: Unknown operator: %c\n", lineno, op);
        exit(1);
    }

    emit_error("Operator '%s' is not supported for '%s and '%s'\n", lookup_op(op), lookup_value_type_name(l->type),
               lookup_value_type_name(r->type));

    return NULL;
}

struct ast *ast_newnode_op(enum op op, struct ast *l, struct ast *r)
{
    struct opnode *a = malloc(sizeof(struct opnode));

    a->nodetype = op;
    a->lineno = yylineno;
    a->l = l;
    a->r = r;

    a->type = T_UNKNOWN;
    a->fn = get_op(a->lineno, op, l, r, &a->type);

    return (struct ast *)a;
}