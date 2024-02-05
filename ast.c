#include "ast.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *error_buf[100];
int error_buf_ptr = 0;

// These are the function bodies that are user defined
// We define them here (instead of in the function table) because we need to keep the AST node
struct routine routines[100];

struct routine *routine_get(struct fn_symbol *fn)
{
    for (int i = 0; i < 100; i++)
        if (routines[i].fn->name && routines[i].fn == fn)
            return &routines[i];

    return NULL;
}

void routine_add(struct fn_symbol *fn, struct ast *block)
{
    for (int i = 0; i < 100; i++)
    {
        if (!routines[i].fn)
        {
            routines[i].fn = fn;
            routines[i].block = block;
            return;
        }
    }
}

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
    printf("%d: Type mismatch: expected %s, but was %s\n", lineno, lookup_value_type_name(expected),
           lookup_value_type_name(was));
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
    sprintf(s, "%d: Operator '%s' is not supported for '%s and '%s'\n", lineno, lookup_op(op),
            lookup_value_type_name(l->type), lookup_value_type_name(r->type));
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

    struct symbol *s = malloc(sizeof(struct symbol));
    s->type = type;
    s->val = malloc(sizeof(union s_val));

    // TODO pass error buffer to symbol_add and print errors here
    a->symbol = symbol_add(sym_name, s);

    return (struct ast *)a;
}

struct ast *ast_newnode_assign(char *sym_name, struct ast *v)
{
    struct symassign *a = malloc(sizeof(struct symassign));
    a->nodetype = ASSIGNMENT;
    a->type = T_VOID;

    struct symbol *s = symbol_get(sym_name);
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
        sprintf(str, "%d: Cannot assign '%s' to '%s'\n", yylineno, lookup_value_type_name(v->type),
                lookup_value_type_name(s->type));
        add_syntax_err(str);
    }

    a->lineno = yylineno;
    a->symbol = s;
    a->v = v;
    return (struct ast *)a;
}

struct ast *ast_newnode_ref(char *sym_name)
{
    struct symref *a = malloc(sizeof(struct symref));
    a->nodetype = REFERENCE;

    struct symbol *s = symbol_get(sym_name);
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
    a->symbol = s;
    return (struct ast *)a;
}

struct ast *ast_newnode_fn_call(char *fn_name, struct ast *args)
{
    struct fn_call *a = malloc(sizeof(struct fn_call));
    a->nodetype = USER_FUNCTION;

    struct fn_symbol *fn = fn_get(fn_name);
    if (!fn)
    {
        char s[100];
        sprintf(s, "%d: Unknown function '%s'\n", yylineno, fn_name);
        add_syntax_err(s);
        a->type = T_UNKNOWN;
        return (struct ast *)a;
    }

    a->type = fn->return_type;
    a->lineno = yylineno;
    a->fn = fn;

    int nargs = 0;
    for (struct ast *arg = args; arg != NULL; arg = arg->l)
    {
        nargs++;
    }

    if (nargs != fn->params_count)
    {
        char s[100];
        sprintf(s, "%d: Function '%s' expects %d arguments, but was given %d\n", yylineno, fn->name, fn->params_count,
                nargs);
        add_syntax_err(s);
    }
    a->args_count = nargs;

    for (struct ast *arg = args; arg != NULL && fn->params != NULL; arg = arg->l)
    {
        struct symbol *param = fn->params[nargs - 1];
        if (arg->r->type != param->type)
        {
            char s[100];
            // TODO we should retain the name of the parameter for more detail in the error message
            sprintf(s, "%d: Argument [%d] must be a '%s', but was '%s'\n", yylineno, nargs,
                    lookup_value_type_name(param->type), lookup_value_type_name(arg->r->type));
            add_syntax_err(s);
        }
        nargs--;
    }

    a->args = args;
    return (struct ast *)a;
}

struct ast *ast_newnode_builtin_fn_call(char *fn_name, struct ast *args)
{
    struct ast *a = ast_newnode_fn_call(fn_name, args);
    a->nodetype = BUILTIN;
    return a;
}

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *block, struct ast *branches)
{
    struct flow *a = malloc(sizeof(struct flow));
    a->nodetype = nodetype;
    a->lineno = yylineno;
    if (condition != NULL && condition->type != T_BOOL)
    {
        char s[100];
        sprintf(s, "%d: Condition must be a '%s', but was '%s'\n", yylineno, lookup_value_type_name(T_BOOL),
                lookup_value_type_name(condition->type));
        add_syntax_err(s);
    }
    a->condition = condition;

    if (branches != NULL && block->type != branches->type)
    {
        char s[100];
        sprintf(s, "%d: Branches must be of the same type ('%s' vs. '%s')\n", yylineno,
                lookup_value_type_name(block->type), lookup_value_type_name(branches->type));
        add_syntax_err(s);
    }

    a->type = block->type;
    a->block = block;
    a->branches = branches;
    return (struct ast *)a;
}

struct ast *ast_newnode_block(struct ast *stmts, struct ast *expr, struct symbol_table_entry *prev_scope)
{
    struct block *a = malloc(sizeof(struct block));
    a->nodetype = BLOCK;

    a->type = T_VOID;
    if (expr != NULL)
    {
        a->type = expr->type;
    }

    a->lineno = yylineno;
    a->stmts = stmts;
    a->expr = expr;

    scope_end(prev_scope);

    return (struct ast *)a;
}

struct ast *ast_newnode_fn_decl(struct fn_symbol *fn, struct ast *block)
{
    if (block != NULL && block->type != fn->return_type)
    {
        char s[100];
        sprintf(s, "%d: Function '%s' return type must be '%s', but was '%s'\n", yylineno, fn->name,
                lookup_value_type_name(fn->return_type), lookup_value_type_name(block->type));
        add_syntax_err(s);
    }

    routine_add(fn, block);

    struct ast *a = malloc(sizeof(struct ast));
    a->nodetype = DECLARATION;
    a->lineno = yylineno;
    a->type = T_VOID;
    return a;
}

struct fn_symbol *function_signature(char *fn_name, struct ast *params, enum value_type type)
{
    struct fn_symbol *fn = fn_get(fn_name);
    if (fn)
    {
        char s[100];
        sprintf(s, "%d: Function '%s' already declared\n", yylineno, fn_name);
        add_syntax_err(s);
        return NULL;
    }

    int params_count = 0;
    for (struct ast *param = params; param != NULL; param = param->l)
    {
        params_count++;
    }

    struct symbol **params_list = malloc(sizeof(struct symbol) * params_count);
    int pc = params_count;
    for (struct ast *param = params; param != NULL; param = param->l)
    {
        struct symdecl *p = ((struct symdecl *)param->r);

        // TODO I am not exactly sure if this will actually let the other nodes see the params by reference...
        params_list[pc - 1] = p->symbol;
        pc--;
    }

    return fn_add(fn_name, type, params_list, params_count);
}

struct ast *ast_newnode_pipe(struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));
    a->nodetype = STATEMENT; // Statement because we just have to evaluate a tree of statements and expressions
    a->lineno = yylineno;
    a->l = l;
    a->r = r;

    a->type = r->type;
    return a;
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
    case BLOCK:
        if (((struct block *)a)->stmts != NULL)
            ast_eval(((struct block *)a)->stmts);

        if (((struct block *)a)->expr != NULL)
            return ast_eval(((struct block *)a)->expr);

        break;
    case DECLARATION:
        return v;
    case ASSIGNMENT:
        ((struct symassign *)a)->symbol->val = ast_eval(((struct symassign *)a)->v);
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
        return ((struct symref *)a)->symbol->val;
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
    {
        struct flow *f = (struct flow *)a;
        // else branch
        if (f->condition == NULL)
        {
            return ast_eval(f->block);
        }

        if (ast_eval(f->condition)->boolean)
        {
            return ast_eval(f->block);
        }
        else
        {
            return ast_eval(f->branches);
        }
    }
    case IF_STMT:
    {
        struct flow *f = (struct flow *)a;
        if (f->condition == NULL)
        {
            ast_eval(f->block);
            break;
        }

        if (ast_eval(f->condition)->boolean)
        {
            if (f->block != NULL)
                ast_eval(f->block);
        }
        else
        {
            if (f->branches != NULL)
            {
                ast_eval(f->branches);
            }
        }
        break;
    }
    case FOR_STMT:
    {
        struct flow *f = (struct flow *)a;
        for (; ast_eval(f->condition)->boolean;)
        {
            ast_eval(f->block);
        }
        break;
    }
    case BUILTIN:
    {
        struct fn_call *call = ((struct fn_call *)a);
        if (strcmp(call->fn->name, "print") == 0)
        {
            printf("%s\n", ast_eval(call->args->r)->str);
            break;
        }
        else if (strcmp(call->fn->name, "print_int") == 0)
        {
            printf("%d\n", ast_eval(call->args->r)->num);
            break;
        }
        else if (strcmp(call->fn->name, "read_int") == 0)
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
        else if (strcmp(call->fn->name, "random_int") == 0)
        {
            struct timespec ts;
            if (clock_gettime(0, &ts) == -1)
            {
                printf("Failed to initialize rand seed\n");
                exit(1);
            }
            srand(ts.tv_nsec);

            int upper = ast_eval(call->args->r)->num;
            int lower = ast_eval(call->args->l->r)->num;

            v->num = (rand() % (upper - lower + 1)) + lower;
            break;
        }
        else
        {
            // normally this is checked when type checking, but who knows
            printf("Unknown built-in function: %s\n", call->fn->name);
            exit(1);
        }
    }
    case USER_FUNCTION:
    {
        struct fn_call *call = (struct fn_call *)a;
        struct routine *r = routine_get(call->fn);

        int params_count = call->fn->params_count;
        union s_val **params_buf = malloc(sizeof(union s_val *) * params_count);

        for (struct ast *arg = call->args; arg != NULL && call->fn->params != NULL; arg = arg->l)
        {
            params_buf[params_count - 1] = call->fn->params[params_count - 1]->val;
            call->fn->params[call->fn->params_count - 1]->val = ast_eval(arg->r);
            params_count--;
        }

        if (r != NULL)
        {
            v = ast_eval(r->block);
        }

        params_count = call->fn->params_count;
        for (struct ast *arg = call->args; arg != NULL && call->fn->params != NULL; arg = arg->l)
        {
            call->fn->params[params_count - 1]->val = params_buf[params_count - 1];
            params_count--;
        }

        break;
    }
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
