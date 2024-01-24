struct ast
{
    int nodetype;
    struct ast *l;
    struct ast *r;
};

struct numval
{
    int nodetype; /* N */
    int number;
};

struct symassign
{
    int nodetype;
    struct symbol *s;
    struct ast *v;
};

struct symref
{
    int nodetype;
    struct symbol *s;
};

struct ast *ast_newnode(int nodetype, struct ast *l, struct ast *r);
struct ast *ast_newnode_num(int d);
struct ast *ast_newnode_assign(struct symbol *s, struct ast *v);
struct ast *ast_newnode_ref(struct symbol *s);

int ast_eval(struct ast *);

void ast_free(struct ast *);