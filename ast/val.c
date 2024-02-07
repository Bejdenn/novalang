#include "val.h"

struct ast *ast_newnode_str(char *s)
{
    struct strval *a = malloc(sizeof(struct strval));
    a->nodetype = T_STR;
    a->type = T_STR;
    a->lineno = yylineno;
    a->str = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_num(int d)
{
    struct numval *a = malloc(sizeof(struct numval));
    a->nodetype = T_INT;
    a->type = T_INT;
    a->lineno = yylineno;
    a->number = d;
    return (struct ast *)a;
}

struct ast *ast_newnode_bool(int b)
{
    struct boolval *a = malloc(sizeof(struct boolval));
    a->nodetype = T_BOOL;
    a->type = T_BOOL;
    a->lineno = yylineno;
    a->boolean = b;
    return (struct ast *)a;
}

struct ast *ast_newnode_array(struct ast *items)
{
    struct arrayval *a = malloc(sizeof(struct arrayval));
    a->nodetype = T_ARRAY;
    a->type = T_ARRAY;
    a->lineno = yylineno;

    if (items != NULL)
    {
        a->type = a->type | items->r->type;
    }

    for (struct ast *i = items; i != NULL; i = i->l)
    {
        if ((a->type ^ T_ARRAY) != i->r->type)
        {
            emit_error("Array items must be of the same type");
            a->type = T_UNKNOWN;
            return (struct ast *)a;
        }
    }

    a->items = items;
    return (struct ast *)a;
}

struct ast *ast_newnode_cast(enum value_type cast_to, struct ast *expr)
{
    struct cast *a = malloc(sizeof(struct cast));
    a->nodetype = CAST;
    a->lineno = yylineno;
    a->cast_to = cast_to;
    a->expr = expr;
    return (struct ast *)a;
}

char *to_string(enum value_type type, union s_val *u)
{
    char *s = malloc(100);
    switch (type)
    {
    case T_INT:
        sprintf(s, "%d", u->num);
        break;
    case T_STR:
        sprintf(s, "%s", u->str);
        break;
    case T_BOOL:
        sprintf(s, "%s", u->boolean ? "true" : "false");
        break;
    default:
        printf("Cannot convert value of type '%s' to string\n", lookup_value_type_name(type));
        exit(1);
    }

    return s;
}

union s_val *cast(enum value_type dest, enum value_type src, union s_val *v)
{
    if (dest == src)
    {
        return v;
    }

    union s_val *u = malloc(sizeof(union s_val));
    if (dest == T_STR)
    {
        u->str = to_string(src, v);
    }
    else
    {
        printf("Cannot cast value of type '%s' to type '%s'\n", lookup_value_type_name(src), lookup_value_type_name(dest));
        exit(1);
    }

    return u;
}

void copy_array(union s_val *dest, union s_val *src)
{
    dest->array = malloc(sizeof(struct array));
    dest->array->size = src->array->size;
    dest->array->items = malloc(sizeof(union s_val) * dest->array->size);
    for (int i = 0; i < dest->array->size; i++)
    {
        dest->array->items[i] = src->array->items[i];
    }
}