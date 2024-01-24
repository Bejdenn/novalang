%{
    #include <stdio.h>
    #include "ast.h"
    #include "symbol.h"
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

%token <s> ID
%token <num> NUM
%token DAA 
%token EOL

%type <a> exp, stmt, shortAssign

%start start

%left '+' '-'

%%

start: /* nothing */
    | stmt EOL { printf("%d\n", ast_eval($1)); }

stmt: shortAssign

shortAssign: ID DAA exp { $$ = ast_newnode_assign($1, $3) }

exp: exp '+' exp { $$ = ast_newnode('+', $1, $3); }
    | NUM { $$ = ast_newnode_num($1); }
%%

int main(void) {
    return yyparse();
}