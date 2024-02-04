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
%token IF ELSE WHEN FOR FN

%type <a> Expression Statements Statement ShortVarDeclaration VarDeclaration
%type <a> VarAssignment BuiltInFnCall IfExpression IfStatement ForStatement 
%type <a> FnDeclaration ParamList FnCall ArgList Arg ExpressionBlock
%type <a> StatementBlock Param 

%type <s> NestedScope

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
    | FnDeclaration
    | FnCall ';'
    | ShortVarDeclaration ';'
    | VarDeclaration ';'
    | VarAssignment ';'
    | BuiltInFnCall ';'

FnDeclaration: FN NestedScope ID '(' ParamList ')' ':' TYPE '{' Statements Expression '}' { $$ = ast_newnode_ufn_decl($3, $5, $8, ast_newnode_fn_block($10, $11, $2));}
    | FN NestedScope ID '(' ParamList ')' ':' TYPE '{' Expression '}' { $$ = ast_newnode_ufn_decl($3, $5, $8, ast_newnode_fn_block(NULL, $10, $2));}

// we can use variable declarations in the param list because function declarations have their own scope
ParamList: ParamList ',' Param { $$ = ast_newnode(ARG_LIST, $1, $3); }
    | Param { $$ = ast_newnode(ARG_LIST, NULL, $1); }
    | %empty { $$ = NULL; }

Param: ID ':' TYPE { $$ = ast_newnode_decl($1, $3, 1); }

FnCall: ID '(' ArgList ')' { $$ = ast_newnode_ufn_call($1, $3); }
    | ID '(' ')' { $$ = ast_newnode_ufn_call($1, NULL); }

ArgList: ArgList ',' Arg { $$ = ast_newnode(ARG_LIST, $1, $3); }
    | Arg { $$ = ast_newnode(ARG_LIST, NULL, $1); }

Arg: Expression 

IfStatement: /* IF Expression '{' '}' { $$ = ast_newnode_flow(IF_STMT, $2, NULL, NULL); }
    |*/ IF Expression StatementBlock { $$ = ast_newnode_flow(IF_STMT, $2, $3, NULL); }
    | IF Expression StatementBlock ELSE StatementBlock { $$ = ast_newnode_flow(IF_STMT, $2, $3, $5); }
    | IF Expression StatementBlock ELSE IfStatement { $$ = ast_newnode_flow(IF_STMT, $2, $3, $5); }
  //| IF Expression StatementBlock ELSE '{' '}' { $$ = ast_newnode_flow(IF_STMT, $2, $3, NULL); }
  //| IF Expression '{' '}' ELSE StatementBlock { /* Create AST node that negates condition */ }

ForStatement: /* FOR Expression '{' '}' { $$ = ast_newnode_flow(FOR_STMT, $2, NULL, NULL);}
    |*/ FOR Expression StatementBlock { $$ = ast_newnode_flow(FOR_STMT, $2, $3, NULL); }

BuiltInFnCall: BUILTIN_FN '(' ArgList ')' { $$ = ast_newnode_builtin($1, $3); }
    | BUILTIN_FN '(' ')' { $$ = ast_newnode_builtin($1, NULL); }

ShortVarDeclaration: ID ':' '=' Expression { ast_newnode_decl($1, T_UNKNOWN, 0); $$ = ast_newnode_assign($1, $4); }

VarDeclaration: ID ':' TYPE { $$ = ast_newnode_decl($1, $3, 0); }

VarAssignment: ID '=' Expression { $$ = ast_newnode_assign($1, $3); }

StatementBlock: NestedScope '{' Statements '}' { $$ = ast_newnode_block($3, NULL, $1); }

Expression: '(' Expression ')' { $$ = $2; }
    | Expression CMP Expression { $$ = ast_newnode_op($2, $1, $3); }
    | Expression MOD Expression { $$ = ast_newnode_op('%', $1, $3); } 
    | Expression '*' Expression { $$ = ast_newnode_op('*', $1, $3); }
    | Expression '/' Expression { $$ = ast_newnode_op('/', $1, $3); }
    | Expression '+' Expression { $$ = ast_newnode_op('+', $1, $3); }
    | Expression '-' Expression { $$ = ast_newnode_op('-', $1, $3); }
    | IfExpression
    | BuiltInFnCall
    | FnCall
    | NUM { $$ = ast_newnode_num($1); }
    | STRING { $$ = ast_newnode_str($1); }
    | BOOL { $$ = ast_newnode_bool($1); }
    | ID { $$ = ast_newnode_ref($1); }

IfExpression: WHEN Expression ExpressionBlock ELSE IfExpression { $$ = ast_newnode_flow(IF_EXPR, $2, $3, $5); }
    | WHEN Expression ExpressionBlock ELSE ExpressionBlock { $$ = ast_newnode_flow(IF_EXPR, $2, $3, $5); }

ExpressionBlock: NestedScope '{' Statements Expression '}' { $$ = ast_newnode_block($3, $4, $1); }
    | NestedScope '{' Expression '}' { $$ = ast_newnode_block(NULL, $3, $1); }

NestedScope: %empty { $$ = nested_scope_start(NULL); }
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

    symbol_table_init();
    fn_table_init();

    success = yyparse();
    fclose(f);
    return success;
}