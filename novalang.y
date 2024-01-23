%union {
    char *str;
    int nr; 
}

%token id op num

%%

ASSIGN: id '=' TERM
TERM: TERM op num
        | num