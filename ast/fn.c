#include "fn.h"

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

struct fn_symbol *function_signature(char *fn_name, struct ast *params, enum value_type type)
{
    struct fn_symbol *fn = fn_get(fn_name, NS_GLOBAL);
    if (fn)
    {
        emit_error("Function '%s' already declared", fn_name);
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

    return fn_add(fn_name, NS_GLOBAL, type, params_list, params_count);
}

struct ast *ast_newnode_fn_call(char *fn_name, enum fn_ns ns, struct ast *args)
{
    struct fn_call *a = malloc(sizeof(struct fn_call));
    a->nodetype = USER_FUNCTION;

    struct fn_symbol *fn = fn_get(fn_name, ns);
    if (!fn)
    {
        emit_error("Unknown function '%s' in namespace '%d' \n", fn_name, ns);
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
        emit_error("Function '%s' expects %d arguments, but was given %d", fn->name, fn->params_count, nargs);
    }
    a->args_count = nargs;

    for (struct ast *arg = args; arg != NULL && fn->params != NULL; arg = arg->l)
    {
        struct symbol *param = fn->params[nargs - 1];
        if (arg->r->type != param->type)
        {
            // TODO we should retain the name of the parameter for more detail in the error message
            emit_error("Argument [%d] must be a '%s', but was '%s'", nargs, lookup_value_type_name(param->type),
                       lookup_value_type_name(arg->r->type));
        }
        nargs--;
    }

    a->args = args;
    return (struct ast *)a;
}

struct ast *ast_newnode_builtin_fn_call(char *fn_name, enum fn_ns ns, struct ast *args)
{
    struct ast *a = ast_newnode_fn_call(fn_name, ns, args);
    a->nodetype = BUILTIN;
    return a;
}

struct ast *ast_newnode_fn_decl(struct fn_symbol *fn, struct ast *block)
{
    if (block != NULL && block->type != fn->return_type)
    {
        emit_error("Function '%s' return type must be '%s', but was '%s'", fn->name,
                   lookup_value_type_name(fn->return_type), lookup_value_type_name(block->type));
    }

    routine_add(fn, block);

    struct ast *a = malloc(sizeof(struct ast));
    a->nodetype = DECLARATION;
    a->lineno = yylineno;
    a->type = T_VOID;
    return a;
}
