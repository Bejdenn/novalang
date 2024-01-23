%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
* TODOS:
* - Add rule to match ints with separators (like 1_000_000)
* - Limit nbo of digits in int
*/

enum yytokentype {
    INT = 258,
    FLOAT,
    STRING,
    IDENTIFIER,
    FUNCTION,
    BOOL,
};

struct t_yylval {
    int int_;
    float flt;
    char * str;
    char * id;
} yylval;

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
ADD        "+"
SUB        "-"
MUL        "*"
DIV        "/"
MOD        "mod"
DECLARE    ":"
ASSIGN     "="
INIT       ":="
IDENTIFIER [a-zA-Z0-9_]+

%%
{STRING} { 
    yylval.str = strdup(yytext+1);
    yylval.str[strlen(yylval.str)-1] = 0;
    return STRING;
}
{INT} { 
    striplz(yytext);
    yylval.int_ = strtol(yytext, NULL, 10);
    return INT; 
}
{FLOAT} { 
    striplz(yytext); 
    yylval.flt = strtof(yytext, NULL);
    return FLOAT;
}
{IDENTIFIER} { 
    yylval.id = strdup(yytext);
    return IDENTIFIER; 
}
"/*" { BEGIN(BLOCK_COMMENT); }
<BLOCK_COMMENT>"*/" { BEGIN(INITIAL); }
"//".*
.
%%

int main (void) {
     int tok;
     while ((tok = yylex())) {
        printf("%d = ", tok);
        switch (tok) {
            case INT:
                printf("%d", yylval.int_);
                break;
            case FLOAT:
                printf("%.10f", yylval.flt);
                break;
            case STRING:
                printf("%s", yylval.str);
                break;
            case IDENTIFIER:
                printf("%s", yylval.id);
                break;
        }
        printf("\n");
     }
}
