#include "ast.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>

extern struct symbol symtab[];

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast *ast_newnode_num(int d)
{
    struct numval *a = malloc(sizeof(struct numval));
    a->nodetype = 'N';
    a->number = d;
    return (struct ast *)a;
}

struct ast *ast_newnode_assign(struct symbol *s, struct ast *v)
{
    struct symassign *a = malloc(sizeof(struct symassign));
    a->nodetype = '=';
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *ast_newnode_ref(struct symbol *s)
{
    struct symref *a = malloc(sizeof(struct symref));
    a->nodetype = 'R';
    a->s = s;
    return (struct ast *)a;
}

int ast_eval(struct ast *a)
{
    switch (a->nodetype)
    {
    case 'S':
        ast_eval(a->r);
        return ast_eval(a->l);
    case '=':
        return ((struct symassign *)a)->s->val = ast_eval(((struct symassign *)a)->v);
    case 'N':
        return ((struct numval *)a)->number;
    case 'R':
        return ((struct symref *)a)->s->val;
    case '+':
        return ast_eval(a->l) + ast_eval(a->r);
    default:
        return -1;
    }
}
