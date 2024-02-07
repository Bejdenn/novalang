#include "declaration.h"
#include <stdio.h>

struct ast *ast_newnode_decl(char *sym_name, enum value_type type)
{
    struct symdecl *a = malloc(sizeof(struct symdecl));
    a->nodetype = DECLARATION;
    a->type = T_VOID;
    a->lineno = yylineno;

    struct symbol *s = malloc(sizeof(struct symbol));
    s->type = type;
    s->val = malloc(sizeof(union s_val));

    enum symbol_err errors;
    struct symbol *symbol = symbol_add(sym_name, s, &errors);
    if (!symbol)
    {
        if ((errors | SYMBOL_ALREADY_DEFINED) == SYMBOL_ALREADY_DEFINED)
        {
            emit_error("Symbol '%s' is already declared", sym_name);
        }
    }
    a->symbol = symbol;

    return (struct ast *)a;
}