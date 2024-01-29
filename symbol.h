struct symbol
{
    char *name;
    int val;
};

#define TBL_SIZE 25
extern struct symbol symtab[TBL_SIZE];

struct symbol *symlookup(char *);
