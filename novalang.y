%{
    #include <stdio.h>
    #include <stdarg.h>
    #include <stdlib.h>
    #include "ast.h"

    int yylex(void);
    extern int yylineno;

    void yyerror(char *s, ...)
    {
        va_list ap;
        va_start(ap, s);
        fprintf(stderr, "%d: error: ", yylineno);
        vfprintf(stderr, s, ap);
        fprintf(stderr, "\n");
    }
%}

%union {
    char *str;
    int num;
    int boolean;
    struct symbol *s;
    struct ast *a;
}

%token <str> ID STRING
%token <num> NUM BUILTIN_FN TYPE
%token <boolean> BOOL
%token IF ELSE FOR

%type <a> Expression Statements Statement ShortVarDeclaration VarDeclaration VarAssignment BuiltInFunction IfExpression IfStatement ForStatement

%start Start

%nonassoc <num> CMP
%left '+' '-'
%left '*' '/' MOD

%%

Start: %empty
    | Statements { ast_interpret($1); printf("Parser: ok\n"); }

Statements: Statements Statement { $$ = ast_newnode(STATEMENT, $1, $2); }
    | Statement

Statement: IfStatement
    | ForStatement
    | ShortVarDeclaration ';'
    | VarDeclaration ';'
    | VarAssignment ';'
    | BuiltInFunction ';'

IfStatement: IF Expression '{' '}' { }
    | IF Expression '{' Statements '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, NULL); }
    | IF Expression '{' Statements '}' ELSE '{' Statements '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, $8); }
    | IF Expression '{' Statements '}' ELSE '{' '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, NULL); }
    | IF Expression '{' '}' ELSE '{' Statements '}' { /* Create AST node that negates condition */ }

ForStatement: FOR Expression '{' '}' { }
    | FOR Expression '{' Statements '}' { $$ = ast_newnode_flow(FOR_STMT, $2, $4, NULL); }

BuiltInFunction: BUILTIN_FN '(' Expression ')' { $$ = ast_newnode_builtin($1, $3); }

ShortVarDeclaration: ID ':' '=' Expression { ast_newnode_decl($1, T_UNKNOWN); $$ = ast_newnode_assign($1, $4); }

VarDeclaration: ID ':' TYPE { $$ = ast_newnode_decl($1, $3); }

VarAssignment: ID '=' Expression { $$ = ast_newnode_assign($1, $3); }

Expression: '(' Expression ')' { $$ = $2; }
    | Expression CMP Expression { $$ = ast_newnode_op($2, $1, $3); }
    | Expression MOD Expression { $$ = ast_newnode_op('%', $1, $3); } 
    | Expression '*' Expression { $$ = ast_newnode_op('*', $1, $3); }
    | Expression '/' Expression { $$ = ast_newnode_op('/', $1, $3); }
    | Expression '+' Expression { $$ = ast_newnode_op('+', $1, $3); }
    | Expression '-' Expression { $$ = ast_newnode_op('-', $1, $3); }
    | IfExpression { $$ = $1; }
    | NUM { $$ = ast_newnode_num($1); }
    | STRING { $$ = ast_newnode_str($1); }
    | BOOL { $$ = ast_newnode_bool($1); }
    | ID { $$ = ast_newnode_ref($1); }

IfExpression: IF Expression '{' Expression '}' ELSE '{' Expression '}' { $$ = ast_newnode_if_expr($2, $4, $8); }

%%

int main(void) {
    return yyparse();
}