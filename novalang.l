%option yylineno noyywrap

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "novalang.tab.h"
#include "ast.h"

/*
* TODOS:
* - Add rule to match ints with separators (like 1_000_000)
* - Limit nbo of digits in int
*/

void striplz(char *str) {
    int negative = *yytext == '-';

    if (negative) {
        yytext++;
    }
    
    while (strlen(yytext) > 1 && *yytext == '0') { yytext++; }

    if (negative) {
        yytext--;
        *yytext = '-';
    }
}

%}

%x BLOCK_COMMENT

INT        -?0*([1-9][0-9]*|0)
STRING     \"[^\"]*\"
BOOL       (true|false)
IDENTIFIER [a-zA-Z0-9_]+

%%
"+" |
"-" |
"*" |
"/" |
";" |
":" |
"(" |
")" |
"{" |
"}" |
"," |
"=" { return yytext[0]; }

"mod" { return MOD; }

"==" { yylval.num = EQ;         return CMP; }
"!=" { yylval.num = N_EQ;       return CMP; }
"<"  { yylval.num = LESS;       return CMP; }
"<=" { yylval.num = LESS_OR_EQ; return CMP; }
">"  { yylval.num = GRT;        return CMP; }
">=" { yylval.num = GRT_OR_EQ;  return CMP; }

"int"  { yylval.num = T_INT;   return TYPE; }
"flt"  { yylval.num = T_FLT;   return TYPE; }
"bool" { yylval.num = T_BOOL;  return TYPE; }
"str"  { yylval.num = T_STR;   return TYPE; }

"if"   { return IF; }
"else" { return ELSE; }
"for"  { return FOR; }

"fn" { return FN; }

{INT} {
    striplz(yytext);
    yylval.num = strtol(yytext, NULL, 10);
    return NUM; 
}

{STRING} { 
    yylval.str = strdup(yytext+1);
    yylval.str[strlen(yylval.str)-1] = 0;
    return STRING;
}

{BOOL} {
    yylval.boolean = strcmp(yytext, "true") == 0;
    return BOOL;
}

"print"     |
"print_int" |
"read_int"  |
"random_int" { yylval.str = strdup(yytext); return BUILTIN_FN; }

{IDENTIFIER} { 
    yylval.str = strdup(yytext);
    return ID; 
}

"/*" { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/" { BEGIN(INITIAL); }
<BLOCK_COMMENT>[.|\n|\t| ]* { }

"//".*

[ |\t|\n|.] { }
%%