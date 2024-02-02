#include "ast.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

char *error_buf[100];
int error_buf_ptr = 0;

void add_syntax_err(char *err)
{
    error_buf[error_buf_ptr++] = strdup(err);
}

void do_arithm_op(union s_val *, int, struct ast *);
void do_cmp(union s_val *, int, struct ast *);

enum value_type op_get_type(int, char, struct ast *, struct ast *);

char *lookup_op(int op)
{
    switch (op)
    {
    case '+':
        return "+";
    case '-':
        return "-";
    case '%':
        return "%";
    case '*':
        return "*";
    case '/':
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

void type_mismatch(int lineno, int expected, int was)
{
    printf("%d: Type mismatch: expected %s, but was %s\n", lineno, lookup_value_type_name(expected), lookup_value_type_name(was));
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

enum value_type op_get_type(int lineno, char op, struct ast *l, struct ast *r)
{
    switch (op)
    {
    case '+':
        switch (l->type)
        {
        case T_INT:
            switch (r->type)
            {
            case T_INT:
                return T_INT;
            case T_STR:
                return T_STR;
            default:
                break;
            }
            break;
        case T_STR:
            switch (r->type)
            {
            case T_INT:
                return T_STR;
            case T_STR:
                return T_STR;
            case T_BOOL:
                return T_STR;
            default:
                break;
            }
            break;
        case T_BOOL:
            switch (r->type)
            {
            case T_STR:
                return T_STR;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case '-':
    case '%':
    case '*':
    case '/':
        if (l->type == T_INT && r->type == T_INT)
        {
            return T_INT;
        }
        break;
    case EQ:
    case N_EQ:
        if (l->type == r->type)
        {
            return T_BOOL;
        }
        break;
    case GRT:
    case LESS:
    case GRT_OR_EQ:
    case LESS_OR_EQ:
        if (l->type == T_INT && r->type == T_INT)
        {
            return T_BOOL;
        }
        break;
    default:
        printf("%d: Unknown operator: %c\n", lineno, op);
        exit(1);
    }

    char s[100];
    sprintf(s, "%d: Operator '%s' is not supported for '%s and '%s'\n", lineno, lookup_op(op), lookup_value_type_name(l->type), lookup_value_type_name(r->type));
    add_syntax_err(s);
    return T_UNKNOWN;
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

struct ast *ast_newnode_op(char op, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    a->nodetype = op;
    a->lineno = yylineno;
    a->l = l;
    a->r = r;
    a->type = op_get_type(a->lineno, op, l, r);

    return a;
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

struct ast *ast_newnode_str(char *s)
{
    struct strval *a = malloc(sizeof(struct strval));
    a->nodetype = T_STR;
    a->type = T_STR;
    a->lineno = yylineno;
    a->str = s;
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

struct ast *ast_newnode_decl(char *sym_name, enum value_type type)
{
    struct symdecl *a = malloc(sizeof(struct symdecl));
    a->nodetype = DECLARATION;
    a->type = T_VOID;
    a->lineno = yylineno;

    if (symlookup(sym_name))
    {
        char str[100];
        sprintf(str, "%d: Symbol '%s' already declared\n", yylineno, sym_name);
        add_syntax_err(str);
    }

    a->s = symadd(sym_name, type);

    return (struct ast *)a;
}

struct ast *ast_newnode_assign(char *sym_name, struct ast *v)
{
    struct symassign *a = malloc(sizeof(struct symassign));
    a->nodetype = ASSIGNMENT;
    a->type = T_VOID;

    struct symbol *s = symlookup(sym_name);
    if (!s)
    {
        char str[100];
        sprintf(str, "%d: Undeclared reference '%s'\n", yylineno, sym_name);
        add_syntax_err(str);
        return (struct ast *)a;
    }

    if (s->type == T_UNKNOWN)
    {
        s->type = v->type;
    }
    else if (s->type != v->type)
    {
        char str[100];
        sprintf(str, "%d: Cannot assign '%s' to '%s'\n", yylineno, lookup_value_type_name(v->type), lookup_value_type_name(s->type));
        add_syntax_err(str);
    }

    a->lineno = yylineno;
    a->s = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *ast_newnode_ref(char *sym_name)
{
    struct symref *a = malloc(sizeof(struct symref));
    a->nodetype = REFERENCE;

    struct symbol *s = symlookup(sym_name);
    if (!s)
    {
        char str[100];
        sprintf(str, "%d: Undeclared reference '%s'\n", yylineno, sym_name);
        add_syntax_err(str);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    a->type = s->type;
    a->lineno = yylineno;
    a->s = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_builtin(char *fn, struct ast *args)
{
    struct builtInFn *a = malloc(sizeof(struct builtInFn));
    a->nodetype = BUILTIN;

    struct fn_symbol *builtin_fn = fnlookup(fn);

    if (!builtin_fn)
    {
        char s[100];
        sprintf(s, "%d: Unknown function '%s'\n", yylineno, fn);
        add_syntax_err(s);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    a->type = builtin_fn->return_type;
    a->lineno = yylineno;
    a->fn = builtin_fn;

    int nargs = 0;
    for (struct ast *arg = args; arg != NULL; arg = arg->l)
    {
        nargs++;
    }

    if (nargs != builtin_fn->params_count)
    {
        char s[100];
        sprintf(s, "%d: Function '%s' expects %d arguments, but was given %d\n", yylineno, builtin_fn->name, builtin_fn->params_count, nargs);
        add_syntax_err(s);
    }

    for (struct ast *arg = args; arg != NULL && builtin_fn->params != NULL; arg = arg->l)
    {
        struct symbol param = builtin_fn->params[nargs - 1];
        if (arg->r->type != param.type)
        {
            char s[100];
            sprintf(s, "%d: Argument '%s' must be a '%s', but was '%s'\n", yylineno, param.name, lookup_value_type_name(param.type), lookup_value_type_name(arg->r->type));
            add_syntax_err(s);
        }
        nargs--;
    }

    a->args = args;
    return (struct ast *)a;
}

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *true_branch, struct ast *false_branch)
{
    struct flow *a = malloc(sizeof(struct flow));
    a->nodetype = nodetype;
    a->lineno = yylineno;
    a->type = T_VOID;
    if (condition->type != T_BOOL)
    {
        char s[100];
        sprintf(s, "%d: Condition must be a '%s', but was '%s'\n", yylineno, lookup_value_type_name(T_BOOL), lookup_value_type_name(condition->type));
        add_syntax_err(s);
    }
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
    if (condition->type != T_BOOL)
    {
        char s[100];
        sprintf(s, "%d: Condition must be a '%s', but was '%s'\n", yylineno, lookup_value_type_name(T_BOOL), lookup_value_type_name(condition->type));
        add_syntax_err(s);
    }
    a->condition = condition;

    if (true_branch->type != false_branch->type)
    {
        char s[100];
        sprintf(s, "%d: Branches must be of the same type ('%s' vs. '%s')\n", yylineno, lookup_value_type_name(true_branch->type), lookup_value_type_name(false_branch->type));
        add_syntax_err(s);
    }

    a->type = true_branch->type;
    a->true_branch = true_branch;
    a->false_branch = false_branch;
    return (struct ast *)a;
}

void ast_interpret(struct ast *a)
{
    if (error_buf_ptr > 0)
    {
        for (int i = 0; i < error_buf_ptr; i++)
        {
            fprintf(stderr, "%s", error_buf[i]);
        }
        exit(1);
    }

    ast_eval(a);
}

union s_val *ast_eval(struct ast *a)
{
    union s_val *v = malloc(sizeof(union s_val));
    switch (a->nodetype)
    {
    case STATEMENT:
        ast_eval(a->l);
        return ast_eval(a->r);
    case DECLARATION:
        return v;
    case ASSIGNMENT:
        struct symassign *asgn = (struct symassign *)a;
        asgn->s->val = ast_eval(((struct symassign *)a)->v);
        break;
    case T_INT:
        v->num = ((struct numval *)a)->number;
        break;
    case T_STR:
        v->str = ((struct strval *)a)->str;
        break;
    case T_BOOL:
        v->boolean = ((struct boolval *)a)->boolean;
        break;
    case REFERENCE:
        v = ((struct symref *)a)->s->val;
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
        if (ast_eval(f->condition)->boolean)
        {
            v = ast_eval(f->true_branch);
        }
        else
        {
            v = ast_eval(f->false_branch);
        }
        break;
    case IF_STMT:
        f = (struct flow *)a;
        if (ast_eval(f->condition)->boolean)
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
        for (; ast_eval(f->condition)->boolean;)
        {
            ast_eval(f->true_branch);
        }
        break;
    case BUILTIN:
        struct builtInFn *bif = ((struct builtInFn *)a);
        if (strcmp(bif->fn->name, "print") == 0)
        {
            printf("%s\n", ast_eval(bif->args->r)->str);
            break;
        }
        else if (strcmp(bif->fn->name, "print_int") == 0)
        {
            printf("%d\n", ast_eval(bif->args->r)->num);
            break;
        }
        else if (strcmp(bif->fn->name, "read_int") == 0)
        {
            char line[128] = {0};
            if (fgets(line, sizeof(line), stdin))
            {
                if (1 != sscanf(line, "%d", &v->num))
                {
                    printf("Invalid input\n");
                    exit(1);
                }
            }
            break;
        }
        else
        {
            // normally this is checked when type checking, but who knows
            printf("Unknown built-in function: %s\n", bif->fn->name);
            exit(1);
        }
        break;
    case ARG_LIST:
        return ast_eval(a->l);
    default:
        printf("Unknown AST node type: %d\n", a->nodetype);
        exit(1);
    }

    return v;
}

void do_arithm_op(union s_val *v, int op, struct ast *a)
{
    union s_val *l_val = ast_eval(a->l);
    union s_val *r_val = ast_eval(a->r);

    switch (op)
    {
    case '+':
        switch (a->l->type)
        {
        case T_INT:
            switch (a->r->type)
            {
            case T_INT:
                v->num = l_val->num + r_val->num;
                break;
            case T_STR:
                v->str = str_concat(to_string(a->l->type, l_val), r_val->str);
                break;
            default:
                break;
            }
            break;
        case T_STR:
            switch (a->r->type)
            {
            case T_INT:
                v->str = str_concat(l_val->str, to_string(a->r->type, r_val));
                break;
            case T_STR:
                v->str = str_concat(l_val->str, r_val->str);
                break;
            case T_BOOL:
                v->str = str_concat(l_val->str, to_string(a->r->type, r_val));
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        break;
    case '-':
        v->num = l_val->num - r_val->num;
        break;
    case '%':
        v->num = l_val->num % r_val->num;
        break;
    case '*':
        v->num = l_val->num * r_val->num;
        break;
    case '/':
        v->num = l_val->num / r_val->num;
        break;
    }
}

void do_cmp(union s_val *v, int op, struct ast *a)
{
    union s_val *l_val = ast_eval(a->l);
    union s_val *r_val = ast_eval(a->r);

    switch (op)
    {
    case EQ:
        switch (a->l->type)
        {
        case T_INT:
            v->boolean = l_val->num == r_val->num;
            break;
        case T_STR:
            v->boolean = strcmp(l_val->str, r_val->str) == 0;
            break;
        case T_BOOL:
            v->boolean = l_val->boolean == r_val->boolean;
            break;
        default:
            break;
        }
        break;
    case N_EQ:
        switch (a->l->type)
        {
        case T_INT:
            v->boolean = l_val->num != r_val->num;
            break;
        case T_STR:
            v->boolean = strcmp(l_val->str, r_val->str) != 0;
            break;
        case T_BOOL:
            v->boolean = l_val->boolean != r_val->boolean;
            break;
        default:
            break;
        }
        break;
    case GRT:
        v->boolean = l_val->num > r_val->num;
        break;
    case LESS:
        v->boolean = l_val->num < r_val->num;
        break;
    case GRT_OR_EQ:
        v->boolean = l_val->num >= r_val->num;
        break;
    case LESS_OR_EQ:
        v->boolean = l_val->num <= r_val->num;
        break;
    default:
        break;
    }
}
