#include "assignment.h"
#include "ref.h"

struct ast *ast_newnode_assign(char *sym_name, struct ast *v)
{
    struct symassign *a = malloc(sizeof(struct symassign));
    a->nodetype = ASSIGNMENT;
    a->type = T_VOID;

    struct symbol *s = symbol_get(sym_name);
    if (!s || !symbol_is_visible(s))
    {
        emit_error("Undeclared reference '%s'", sym_name);
        return (struct ast *)a;
    }

    if (s->type == T_UNKNOWN)
    {
        s->type = v->type;
    }
    else if (s->type != v->type)
    {
        emit_error("Cannot assign '%s' to '%s'", lookup_value_type_name(v->type), lookup_value_type_name(s->type));
    }

    a->lineno = yylineno;
    a->symbol = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *ast_newnode_index_assign(char *sym_name, struct ast *index, struct ast *v)
{
    struct ast *a = ast_newnode_index(sym_name, index);
    struct indexassign *arr_assign = (struct indexassign *)a;
    arr_assign->nodetype = INDEX_ASSIGNMENT;
    arr_assign->v = v;

    if (a->type != v->type)
    {
        emit_error("Cannot assign '%s' to '%s'", lookup_value_type_name(v->type), lookup_value_type_name(a->type));
    }

    return a;
}