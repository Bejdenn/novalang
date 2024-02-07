#include "ref.h"

struct ast *ast_newnode_ref(char *sym_name)
{
    struct symref *a = malloc(sizeof(struct symref));
    a->nodetype = REFERENCE;

    struct symbol *s = symbol_get(sym_name);
    if (!s || !symbol_is_visible(s))
    {
        emit_error("Undeclared reference '%s'", sym_name);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    a->type = s->type;
    a->lineno = yylineno;
    a->symbol = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_index(char *sym_name, struct ast *index)
{
    struct symindex *a = malloc(sizeof(struct symindex));
    a->nodetype = INDEX;

    struct symbol *s = symbol_get(sym_name);
    if (!s || !symbol_is_visible(s))
    {
        emit_error("Undeclared reference '%s'", sym_name);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    if ((s->type & T_ARRAY) != T_ARRAY)
    {
        emit_error("'%s' is not an array", sym_name);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    if (index->type != T_INT)
    {
        emit_error("Index must be an integer, but was '%s'", lookup_value_type_name(index->type));
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    a->type = s->type ^ T_ARRAY;
    a->lineno = yylineno;
    a->symbol = s;
    a->index = index;
    return (struct ast *)a;
}