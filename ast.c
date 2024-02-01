#include "ast.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

extern struct symbol symtab[];

void do_arithm_op(struct value *, int, struct ast *);
void do_cmp(struct value *, int, struct ast *);

void type_mismatch(int lineno, int expected, int was)
{
    printf("%d: Type mismatch: expected %s, but was %s\n", lineno, lookup_value_type_name(expected), lookup_value_type_name(was));
    exit(1);
}

char *to_string(struct value *v)
{
    char *s = malloc(100);
    switch (v->type)
    {
    case T_INT:
        sprintf(s, "%d", v->u->num);
        break;
    case T_STR:
        sprintf(s, "%s", v->u->str);
        break;
    case T_BOOL:
        sprintf(s, "%s", v->u->boolean ? "true" : "false");
        break;
    default:
        // TODO we should never get here
        sprintf(s, "Unknown type: %d", v->type);
        break;
    }

    return s;
}

char *str_concat(char *s1, char *s2)
{
    char *s = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(s, s1);
    strcat(s, s2);
    return s;
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

struct ast *ast_newnode_num(int d)
{
    struct numval *a = malloc(sizeof(struct numval));
    a->nodetype = T_INT;
    a->lineno = yylineno;
    a->number = d;
    return (struct ast *)a;
}

struct ast *ast_newnode_str(char *s)
{
    struct strval *a = malloc(sizeof(struct strval));
    a->nodetype = T_STR;
    a->lineno = yylineno;
    a->str = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_bool(int b)
{
    struct boolval *a = malloc(sizeof(struct boolval));
    a->nodetype = T_BOOL;
    a->lineno = yylineno;
    a->boolean = b;
    return (struct ast *)a;
}

struct ast *ast_newnode_decl(struct symbol *s)
{
    struct symdecl *a = malloc(sizeof(struct symdecl));
    a->nodetype = DECLARATION;
    a->lineno = yylineno;
    a->s = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_assign(struct symbol *s, struct ast *v)
{
    struct symassign *a = malloc(sizeof(struct symassign));
    a->nodetype = ASSIGNMENT;
    a->lineno = yylineno;
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *ast_newnode_ref(struct symbol *s)
{
    struct symref *a = malloc(sizeof(struct symref));
    a->nodetype = REFERENCE;
    a->lineno = yylineno;
    a->s = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_builtin(int fn, struct ast *args)
{
    struct builtInFn *a = malloc(sizeof(struct builtInFn));
    a->nodetype = BUILTIN;
    a->lineno = yylineno;
    a->fn = fn;
    a->args = args;
    return (struct ast *)a;
}

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *true_branch, struct ast *false_branch)
{
    struct flow *a = malloc(sizeof(struct flow));
    a->nodetype = nodetype;
    a->lineno = yylineno;
    a->condition = condition;
    a->true_branch = true_branch;
    a->false_branch = false_branch;
    return (struct ast *)a;
}

struct ast *ast_newnode_if_expr(struct ast *condition, struct ast *true_branch, struct ast *false_branch)
{
    struct flow *a = malloc(sizeof(struct flow));
    a->nodetype = IF_EXPR;
    a->lineno = yylineno;
    a->condition = condition;
    a->true_branch = true_branch;
    a->false_branch = false_branch;
    return (struct ast *)a;
}

struct value *ast_eval(struct ast *a)
{
    struct value *v = malloc(sizeof(struct value));
    v->u = malloc(sizeof(union s_val));
    v->type = T_VOID;
    switch (a->nodetype)
    {
    case STATEMENT:
        ast_eval(a->l);
        return ast_eval(a->r);
    case DECLARATION:
        return v;
    case ASSIGNMENT:
        struct symassign *one = (struct symassign *)a;
        struct value *other = ast_eval(((struct symassign *)a)->v);
        // Support walrus operator by having it tagged as unknown until value is assigned
        if (one->s->type == T_UNKNOWN)
        {
            one->s->type = other->type;
        }
        else if (one->s->type != other->type)
        {
            type_mismatch(one->lineno, one->s->type, other->type);
            exit(1);
        }

        one->s->val = other->u;
        break;
    case T_INT:
        v->type = T_INT;
        v->u->num = ((struct numval *)a)->number;
        break;
    case T_STR:
        v->type = T_STR;
        v->u->str = ((struct strval *)a)->str;
        break;
    case T_BOOL:
        v->type = T_BOOL;
        v->u->boolean = ((struct boolval *)a)->boolean;
        break;
    case REFERENCE:
        v->type = ((struct symref *)a)->s->type;
        v->u = ((struct symref *)a)->s->val;
        break;
    case '+':
    case '-':
    case '%':
    case '*':
    case '/':
        do_arithm_op(v, a->nodetype, a);
        break;
    case EQ:
    case N_EQ:
    case GRT:
    case LESS:
    case GRT_OR_EQ:
    case LESS_OR_EQ:
        do_cmp(v, a->nodetype, a);
        break;
    case IF_EXPR:
        struct flow *f = (struct flow *)a;
        struct value *cond = ast_eval(f->condition);
        if (cond->type != T_BOOL)
        {
            printf("Condition must be a boolean\n");
            exit(1);
        }
        struct value *true_v = ast_eval(f->true_branch);
        struct value *false_v = ast_eval(f->false_branch);
        if (true_v->type != false_v->type)
        {
            type_mismatch(f->lineno, true_v->type, false_v->type);
            exit(1);
        }
        if (cond->u->boolean)
        {
            v = true_v;
        }
        else
        {
            v = false_v;
        }
        break;
    case IF_STMT:
        f = (struct flow *)a;
        cond = ast_eval(f->condition);
        if (cond->type != T_BOOL)
        {
            printf("Condition must be a boolean\n");
            exit(1);
        }
        if (cond->u->boolean)
        {
            ast_eval(f->true_branch);
        }
        else
        {
            if (f->false_branch != NULL)
            {
                ast_eval(f->false_branch);
            }
        }
        break;
    case FOR_STMT:
        f = (struct flow *)a;
        for (; ast_eval(f->condition)->u->boolean;)
        {
            ast_eval(f->true_branch);
        }
        break;
    case BUILTIN:
        struct builtInFn *node = ((struct builtInFn *)a);
        switch (node->fn)
        {
        case PRINT:
            printf("%s\n", to_string(ast_eval(node->args)));
        }
        break;
    default:
        printf("Unknown AST node type: %d\n", a->nodetype);
        exit(1);
    }

    return v;
}

void do_arithm_op(struct value *v, int op, struct ast *a)
{
    v->type = T_INT;
    struct value *l_val = ast_eval(a->l);
    struct value *r_val = ast_eval(a->r);

    switch (op)
    {
    case '+':
        if (l_val->type == T_STR)
        {
            v->type = T_STR;
            v->u->str = str_concat(l_val->u->str, to_string(r_val));
            break;
        }
        else if (r_val->type == T_STR)
        {
            v->type = T_STR;
            v->u->str = str_concat(to_string(l_val), r_val->u->str);
            break;
        }

        if (l_val->type == T_INT && r_val->type == T_INT)
        {
            v->u->num = l_val->u->num + r_val->u->num;
            break;
        }
        else
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        break;
    case '-':
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->num = l_val->u->num - r_val->u->num;
        break;
    case '%':
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->num = l_val->u->num % r_val->u->num;
        break;
    case '*':
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->num = l_val->u->num * r_val->u->num;
        break;
    case '/':
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->num = l_val->u->num / r_val->u->num;
        break;
    }
}

void do_cmp(struct value *v, int op, struct ast *a)
{
    v->type = T_BOOL;
    struct value *l_val = ast_eval(a->l);
    struct value *r_val = ast_eval(a->r);

    switch (op)
    {
    case EQ:
        if (l_val->type != r_val->type)
        {
            v->u->boolean = 0;
            break;
        }
        switch (l_val->type)
        {
        case T_INT:
            v->u->boolean = l_val->u->num == r_val->u->num;
            break;
        case T_STR:
            v->u->boolean = strcmp(l_val->u->str, r_val->u->str) == 0;
            break;
        case T_BOOL:
            v->u->boolean = l_val->u->boolean == r_val->u->boolean;
            break;
        }
        break;
    case N_EQ:
        if (l_val->type != r_val->type)
        {
            v->u->boolean = 1;
            break;
        }
        switch (l_val->type)
        {
        case T_INT:
            v->u->boolean = l_val->u->num != r_val->u->num;
            break;
        case T_STR:
            v->u->boolean = strcmp(l_val->u->str, r_val->u->str) != 0;
            break;
        case T_BOOL:
            v->u->boolean = l_val->u->boolean != r_val->u->boolean;
            break;
        }
        break;
    case GRT:
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->boolean = l_val->u->num > r_val->u->num;
        break;
    case LESS:
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->boolean = l_val->u->num < r_val->u->num;
        break;
    case GRT_OR_EQ:
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->boolean = l_val->u->num >= r_val->u->num;
        break;
    case LESS_OR_EQ:
        if (l_val->type != T_INT || r_val->type != T_INT)
        {
            type_mismatch(a->lineno, l_val->type, r_val->type);
            exit(1);
        }
        v->u->boolean = l_val->u->num <= r_val->u->num;
        break;
    }
}
