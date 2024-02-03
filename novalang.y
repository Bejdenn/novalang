%{
    #include <stdio.h>
    #include <stdarg.h>
    #include <stdlib.h>
    #include "ast.h"

    int yylex(void);
    extern int yylineno;
    extern FILE *yyin;

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

%token <str> ID STRING BUILTIN_FN
%token <num> NUM TYPE
%token <boolean> BOOL
%token IF ELSE FOR FN

%type <a> Expression Statements Statement ShortVarDeclaration VarDeclaration
%type <a> VarAssignment BuiltInFnCall IfExpression IfStatement ForStatement 
%type <a> /* FnDeclaration ParamList FnCall */ ArgList Arg ExpressionBlock

%start Start

%nonassoc <num> CMP
%left '+' '-'
%left '*' '/' MOD

%%

Start: %empty
    | Statements { ast_interpret($1); }

Statements: Statements Statement { $$ = ast_newnode(STATEMENT, $1, $2); }
    | Statement

Statement: IfStatement
    | ForStatement
//    | FnDeclaration
//    | FnCall ';'
    | ShortVarDeclaration ';'
    | VarDeclaration ';'
    | VarAssignment ';'
    | BuiltInFnCall ';'

// FnDeclaration: FN ID '(' ParamList ')' ':' TYPE '{' Statements '}' { }

// ParamList: ParamList ',' ID ':' TYPE { }
//    | ID ':' TYPE { }
//    | %empty { }

// FnCall: ID '(' ArgList ')' {  }

ArgList: ArgList ',' Arg { $$ = ast_newnode(ARG_LIST, $1, $3); }
    | Arg { $$ = ast_newnode(ARG_LIST, NULL, $1); }

Arg: Expression 

IfStatement: IF Expression '{' '}' { $$ = ast_newnode_flow(IF_STMT, $2, NULL, NULL); }
    | IF Expression '{' Statements '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, NULL); }
    | IF Expression '{' Statements '}' ELSE '{' Statements '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, $8); }
    | IF Expression '{' Statements '}' ELSE IfStatement { $$ = ast_newnode_flow(IF_STMT, $2, $4, $7); }
    | IF Expression '{' Statements '}' ELSE '{' '}' { $$ = ast_newnode_flow(IF_STMT, $2, $4, NULL); }
    | IF Expression '{' '}' ELSE '{' Statements '}' { /* Create AST node that negates condition */ }

ForStatement: FOR Expression '{' '}' { $$ = ast_newnode_flow(FOR_STMT, $2, NULL, NULL);}
    | FOR Expression '{' Statements '}' { $$ = ast_newnode_flow(FOR_STMT, $2, $4, NULL); }

BuiltInFnCall: BUILTIN_FN '(' ArgList ')' { $$ = ast_newnode_builtin($1, $3); }
    | BUILTIN_FN '(' ')' { $$ = ast_newnode_builtin($1, NULL); }

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
    | IfExpression
    | BuiltInFnCall
    | NUM { $$ = ast_newnode_num($1); }
    | STRING { $$ = ast_newnode_str($1); }
    | BOOL { $$ = ast_newnode_bool($1); }
    | ID { $$ = ast_newnode_ref($1); }

IfExpression: IF Expression ExpressionBlock ELSE IfExpression { $$ = ast_newnode_flow(IF_EXPR, $2, $3, $5); }
    | IF Expression ExpressionBlock ELSE ExpressionBlock { $$ = ast_newnode_flow(IF_EXPR, $2, $3, $5); }

ExpressionBlock: '{' Statements Expression '}' { $$ = ast_newnode_exprblock($2, $3); }
    | '{' Expression '}' { $$ = $2; }
%%

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <input file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }

    int success = 0;    
    yyin = f;
    success = yyparse();
    fclose(f);
    return success;
}