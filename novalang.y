%{
    #include <stdio.h>
    #include "ast.h"
    #include "symbol.h"
    extern struct symbol symtab[];
    int yylex(void);
    void yyerror (char *msg) {
        fprintf(stderr, "%s\n", msg);
    } 
%}

%union {
    char *str;
    int num;
    struct symbol *s;
    struct ast *a;
}

%token <s> ID <num> NUM

%type <a> Expression Statements Statement VarShortAssign

%start Start

%left '+'

%%

Start: Statements { printf("Out: %d\n", ast_eval($1)); }

Statements: Statements Statement ';' { $$ = ast_newnode('S', $1, $2); }
    | Statement ';'

Statement: VarShortAssign

VarShortAssign: ID ':' '=' Expression { $$ = ast_newnode_assign($1, $4); }

Expression: Expression '+' Expression { $$ = ast_newnode('+', $1, $3); }
    | NUM { $$ = ast_newnode_num($1); }
    | ID { $$ = ast_newnode_ref($1); }

%%

int main(void) {
    return yyparse();
}