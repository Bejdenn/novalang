#include "ast.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

void ast_interpret(struct ast *a)
{
    if (error_count > 0)
    {
        for (int i = 0; i < error_count; i++)
        {
            fprintf(stderr, "%s", errors[i]);
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
        v = ast_eval(((struct symassign *)a)->v);
        *((struct symassign *)a)->symbol->val = *v;

        // arrays have to be copied
        if (((struct symassign *)a)->symbol->type & T_ARRAY)
        {
            copy_array(((struct symassign *)a)->symbol->val, v);
        }
        break;
    case INDEX_ASSIGNMENT:
    {
        struct indexassign *i = (struct indexassign *)a;
        int index = ast_eval(i->index)->num;

        if (index >= i->symbol->val->array->size)
        {
            fprintf(stderr, "Index out of bounds");
            exit(1);
        }

        i->symbol->val->array->items[index] = *ast_eval(i->v);
        break;
    }
    case T_INT:
        v->num = ((struct numval *)a)->number;
        break;
    case T_STR:
        v->str = ((struct strval *)a)->str;
        break;
    case CAST:
        v = cast(((struct cast *)a)->cast_to, ((struct cast *)a)->expr->type, ast_eval(((struct cast *)a)->expr));
        break;
    case T_BOOL:
        v->boolean = ((struct boolval *)a)->boolean;
        break;
    case T_ARRAY:
    {
        struct arrayval *arr = (struct arrayval *)a;
        int size = 0;
        for (struct ast *i = arr->items; i != NULL; i = i->l)
        {
            size++;
        }

        v->array = malloc(sizeof(struct array));
        v->array->items = malloc(sizeof(union s_val) * size);
        v->array->size = size;
        int i = size - 1;
        for (struct ast *item = arr->items; item != NULL; item = item->l)
        {
            v->array->items[i] = *ast_eval(item->r);
        }
        break;
    }
    case REFERENCE:
        return ((struct symref *)a)->symbol->val;
    case INDEX:
    {
        struct symindex *i = (struct symindex *)a;
        int index = ast_eval(i->index)->num;
        return &i->symbol->val->array->items[index];
    }
    case ADD:
    case MINUS:
    case MOD:
    case MUL:
    case DIV:
    case EQ:
    case N_EQ:
    case GRT:
    case LESS:
    case GRT_OR_EQ:
    case LESS_OR_EQ:
        struct opnode *op = ((struct opnode *)a);
        *v = *(op->fn)(ast_eval(op->l), ast_eval(op->r));
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
        if (strcmp(call->fn->name, "print") == 0 && call->fn->ns == NS_STR)
        {
            printf("%s\n", ast_eval(call->args->r)->str);
            break;
        }
        else if (strcmp(call->fn->name, "read") == 0 && call->fn->ns == (NS_STR | NS_ARRAY))
        {
            char line[128] = {0};
            v->array = malloc(sizeof(struct array));
            v->array->items = malloc(sizeof(union s_val));
            int count = 0;

            FILE *file = fopen(ast_eval(call->args->r)->str, "r");
            if (file == NULL)
            {
                fprintf(stderr, "Failed to open file\n");
                exit(1);
            }

            while (fgets(line, sizeof(line), file) != NULL)
            {
                v->array->items[count].str = strdup(line);
                char *str = v->array->items[count].str;
                str[strlen(str) - 1] = 0;

                count++;
                v->array->items = realloc(v->array->items, sizeof(union s_val) * (count + 1));
            }
            v->array->size = count;
            break;
        }
        else if (strcmp(call->fn->name, "len") == 0 && call->fn->ns == (NS_STR | NS_ARRAY))
        {
            v->num = ast_eval(call->args->r)->array->size;
            break;
        }
        else if (strcmp(call->fn->name, "upcase") == 0 && call->fn->ns == NS_STR)
        {
            v->str = strdup(ast_eval(call->args->r)->str);
            for (int i = 0; i < strlen(v->str); i++)
            {
                v->str[i] = toupper(v->str[i]);
            }
            break;
        }
        else if (strcmp(call->fn->name, "split") == 0 && call->fn->ns == NS_STR)
        {
            struct array *array = malloc(sizeof(struct array));
            array->items = malloc(sizeof(union s_val));
            int count = 0;
            char *token = strtok(ast_eval(call->args->l->r)->str, ast_eval(call->args->r)->str);
            while (token != NULL)
            {
                array->items[count].str = strdup(token);
                count++;
                array->items = realloc(array->items, sizeof(union s_val) * (count + 1));
                token = strtok(NULL, ast_eval(call->args->r)->str);
            }
            array->size = count;
            v->array = array;
            break;
        }
        else if (strcmp(call->fn->name, "print") == 0 && call->fn->ns == (NS_STR | NS_ARRAY))
        {
            struct array *array = ast_eval(call->args->r)->array;
            for (int i = 0; i < array->size; i++)
            {
                printf("%s ", array->items[i].str);
            }
            printf("\n");
            break;
        }
        else if (strcmp(call->fn->name, "print") == 0 && call->fn->ns == NS_INT)
        {
            printf("%d\n", ast_eval(call->args->r)->num);
            break;
        }
        else if (strcmp(call->fn->name, "read") == 0 && call->fn->ns == NS_INT)
        {
            char line[128] = {0};
            if (fgets(line, sizeof(line), stdin))
            {
                if (1 != sscanf(line, "%d", &v->num))
                {
                    fprintf(stderr, "Invalid input\n");
                    exit(1);
                }
            }
            break;
        }
        else if (strcmp(call->fn->name, "random") == 0 && call->fn->ns == NS_INT)
        {
            struct timespec ts;
            if (clock_gettime(0, &ts) == -1)
            {
                fprintf(stderr, "Failed to initialize rand seed\n");
                exit(1);
            }
            srand(ts.tv_nsec);

            int upper = ast_eval(call->args->r)->num;
            int lower = ast_eval(call->args->l->r)->num;

            v->num = (rand() % (upper - lower + 1)) + lower;
            break;
        }
        else if (strcmp(call->fn->name, "read") == 0 && call->fn->ns == (NS_INT | NS_ARRAY))
        {
            char line[128] = {0};
            v->array = malloc(sizeof(struct array));
            v->array->items = malloc(sizeof(union s_val));
            int count = 0;
            if (fgets(line, sizeof(line), stdin))
            {
                int i = 0;
                char *token = strtok(line, " ");
                while (token != NULL)
                {
                    if (1 != sscanf(token, "%d", &v->array->items[i++].num))
                    {
                        fprintf(stderr, "Invalid input\n");
                        exit(1);
                    }
                    count++;
                    v->array->items = realloc(v->array->items, sizeof(union s_val) * (count + 1));
                    token = strtok(NULL, " ");
                }
            }
            v->array->size = count;
            break;
        }
        else if (strcmp(call->fn->name, "print") == 0 && call->fn->ns == (T_INT | NS_ARRAY))
        {
            struct array *array = ast_eval(call->args->r)->array;
            for (int i = 0; i < array->size; i++)
            {
                printf("%d ", array->items[i].num);
            }
            printf("\n");
            break;
        }
        else if (strcmp(call->fn->name, "len") == 0 && call->fn->ns == (T_INT | NS_ARRAY))
        {
            v->num = ast_eval(call->args->r)->array->size;
        }
        else
        {
            // normally this is checked when type checking, but who knows
            fprintf(stderr, "Unknown built-in function: %s\n", call->fn->name);
            exit(1);
        }
        break;
    }
    case USER_FUNCTION:
    {
        struct fn_call *call = (struct fn_call *)a;
        struct routine *r = routine_get(call->fn);

        int params_count = call->fn->params_count;
        union s_val **params_buf = malloc(sizeof(union s_val *) * params_count);

        for (struct ast *arg = call->args; arg != NULL && call->fn->params != NULL; arg = arg->l)
        {
            struct symbol *param = call->fn->params[params_count - 1];
            union s_val *arg_val = ast_eval(arg->r);

            params_buf[params_count - 1] = param->val;
            param->val = malloc(sizeof(*params_buf[params_count - 1]));
            *param->val = *arg_val;

            if (param->type & T_ARRAY)
            {
                copy_array(param->val, arg_val);
            }

            params_count--;
        }

        union s_val **symbol_buf[TBL_SIZE];
        for (int i = 0; i < TBL_SIZE; i++)
        {
            int stack_size = ((struct block *)r->block)->scope[i].references->size;
            symbol_buf[i] = malloc(sizeof(union s_val *) * stack_size);
            for (int j = 0; j < stack_size; j++)
            {
                struct symbol *current_s = ((struct block *)r->block)->scope[i].references->val[j];
                int is_param = 0;
                for (int k = 0; k < call->fn->params_count; k++)
                {
                    if (call->fn->params[k] == current_s)
                    {
                        is_param = 1;
                        break;
                    }
                }

                if (is_param)
                {
                    continue;
                }

                // skip global values
                if (current_s->scope == S_GLOBAL_SCOPE)
                {
                    continue;
                }

                symbol_buf[i][j] = current_s->val;
                current_s->val = malloc(sizeof(union s_val));
            }
        }

        if (r != NULL)
        {
            v = ast_eval(r->block);
        }

        // restore the symbol table
        for (int i = 0; i < TBL_SIZE; i++)
        {
            int stack_size = ((struct block *)r->block)->scope[i].references->size;
            for (int j = 0; j < stack_size; j++)
            {
                struct symbol *current_s = ((struct block *)r->block)->scope[i].references->val[j];

                int is_param = 0;
                for (int k = 0; k < call->fn->params_count; k++)
                {
                    if (call->fn->params[k] == current_s)
                    {
                        is_param = 1;
                        break;
                    }
                }

                if (is_param)
                {
                    continue;
                }

                if (current_s->scope == S_GLOBAL_SCOPE)
                {
                    continue;
                }

                current_s->val = symbol_buf[i][j];
            }
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
        fprintf(stderr, "Unknown AST node type: %d\n", a->nodetype);
        exit(1);
    }

    return v;
}