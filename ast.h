#include "symbol.h"

extern int yylineno;

enum nodetype
{
    STATEMENT = 0,
    DECLARATION,
    ASSIGNMENT,
    REFERENCE,
    BUILTIN,
    IF_STMT,
    IF_EXPR,
    FOR_STMT,
    ARG_LIST,
    BLOCK,
    USER_FUNCTION,
    FN_BLOCK,
    INDEX,
    INDEX_ASSIGNMENT
};

enum op
{
    ADD = 15,
    MINUS,
    MUL,
    DIV,
    GRT,
    MOD,
    EQ,
    LESS,
    GRT_OR_EQ,
    LESS_OR_EQ,
    N_EQ
};

struct ast
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *l;
    struct ast *r;
};

struct numval
{
    int nodetype;
    int lineno;
    enum value_type type;
    int number;
};

struct strval
{
    int nodetype;
    int lineno;
    enum value_type type;
    char *str;
};

struct boolval
{
    int nodetype;
    int lineno;
    enum value_type type;
    int boolean;
};

struct arrayval {
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *items;
};

struct symdecl
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
};

struct symassign
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *v;
};

struct symref
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
};

struct symindex
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *index;
};

struct indexassign
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol *symbol;
    struct ast *index;
    struct ast *v;
};

struct arglist
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *expr;
    struct arglist *next;
};

struct routine
{
    struct fn_symbol *fn;
    struct ast *block;
};

struct fn_call
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct fn_symbol *fn;
    struct ast *args;
    int args_count;
};

struct flow
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *condition;
    struct ast *block;
    struct ast *branches;
};

struct block
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct symbol_table_entry *scope;
    struct ast *stmts;
    struct ast *expr;
};

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r);

struct ast *ast_newnode_op(enum op op, struct ast *l, struct ast *r);

struct ast *ast_newnode_num(int d);

struct ast *ast_newnode_str(char *s);

struct ast *ast_newnode_bool(int b);

struct ast *ast_newnode_array(struct ast *items);

struct ast *ast_newnode_decl(char *sym_name, enum value_type type);

struct ast *ast_newnode_assign(char *sym_name, struct ast *v);

struct ast *ast_newnode_index_assign(char *sym_name, struct ast *index, struct ast *v);

struct ast *ast_newnode_ref(char *sym_name);

struct ast* ast_newnode_index(char *sym_name, struct ast *index);

struct ast *ast_newnode_builtin_fn_call(char *fn, struct ast *args);

struct ast *ast_newnode_fn_call(char *fn, struct ast *args);

struct ast *ast_newnode_fn_decl(struct fn_symbol *fn_sign, struct ast *block);

struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *block, struct ast *branches);

struct ast *ast_newnode_exprblock(struct ast *stmts, struct ast *expr);

struct ast *ast_newnode_block(struct ast *stmts, struct ast *expr, struct symbol_table_entry *scope, enum scope_type scope_to_exit);

struct fn_symbol *function_signature(char *fn_name, struct ast *params, enum value_type type);

struct ast *ast_newnode_ufn_call(char *fn_name, struct ast *args);

struct ast *ast_newnode_pipe(struct ast *l, struct ast *r);

void ast_interpret(struct ast *);

union s_val *ast_eval(struct ast *);

void ast_free(struct ast *);