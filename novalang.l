%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "novalang.tab.h"
#include "symbol.h"
#include "ast.h"

/*
* TODOS:
* - Add rule to match ints with separators (like 1_000_000)
* - Limit nbo of digits in int
*/

void striplz(char *str) {
    while (strlen(yytext) > 1 && *yytext == '0') { yytext++; }
}

%}

%x BLOCK_COMMENT

STRING     \"[^\"]*\"
INT        0*([1-9][0-9]*|0)
FLOAT      ({INT}*\.[0-9]+|{INT}+\.)
FUNCTION   "fn"
BOOL       "bool"
TRUE       "true"
FALSE      "false"
MOD        "mod"
DECLARE    ":"
ASSIGN     "="
INIT       ":="
IDENTIFIER [a-zA-Z0-9_]+

%%
"+" { return yytext[0]; }
":=" { return DAA; }
{INT} { 
    striplz(yytext);
    yylval.num = strtol(yytext, NULL, 10);
    return NUM; 
}
{IDENTIFIER} { 
    yylval.s = symlookup(yytext);
    return ID; 
}
\n { return EOL; }
"/*" { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/" { BEGIN(INITIAL); }
"//".*
.
%%
