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
};

enum cmp
{
    EQ = 10,
    GRT,
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

struct symdecl
{
    int nodetype;
    int lineno;
    enum value_type type;
    char *symbol;
};

struct symassign
{
    int nodetype;
    int lineno;
    enum value_type type;
    char *symbol;
    struct ast *v;
};

struct symref
{
    int nodetype;
    int lineno;
    enum value_type type;
    char *symbol;
};

struct arglist
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct ast *expr;
    struct arglist *next;
};

struct builtInFn
{
    int nodetype;
    int lineno;
    enum value_type type;
    struct fn_symbol *fn;
    struct ast *args;
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
    struct symbol *scope;
    struct ast *stmts;
    struct ast *expr;
};

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r);
struct ast *ast_newnode_op(char op, struct ast *l, struct ast *r);
struct ast *ast_newnode_num(int d);
struct ast *ast_newnode_str(char *s);
struct ast *ast_newnode_bool(int b);
struct ast *ast_newnode_decl(char *sym_name, enum value_type type);
struct ast *ast_newnode_assign(char *sym_name, struct ast *v);
struct ast *ast_newnode_ref(char *sym_name);
struct ast *ast_newnode_builtin(char *fn, struct ast *args);
struct ast *ast_newnode_flow(int nodetype, struct ast *condition, struct ast *block, struct ast *branches);
struct ast *ast_newnode_exprblock(struct ast *stmts, struct ast *expr);
struct ast *ast_newnode_block(struct ast *stmts, struct ast *expr, struct symbol *old_t);

void ast_interpret(struct ast *);
union s_val *ast_eval(struct ast *);

void ast_free(struct ast *);