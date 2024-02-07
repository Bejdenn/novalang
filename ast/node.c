#include "node.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char *errors[100];
int error_count = 0;

void emit_error(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    char *err = malloc(100);
    sprintf(err, "%d: ", yylineno);
    vsprintf(err + strlen(err), s, ap);
    sprintf(err + strlen(err), "\n");
    errors[error_count++] = err;
}

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    a->nodetype = nodetype;
    a->lineno = yylineno;
    a->l = l;
    a->r = r;
    return a;
}