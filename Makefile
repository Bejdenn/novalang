CC=gcc
CFLAGS=-O2 -Wall --debug

OBJ=novalang.tab.o lex.yy.o symbol.o ast.o

novalang: $(OBJ)
	$(CC) -o $@ $(OBJ) -ll

%.o: %.c %.h

novalang.tab.c novalang.tab.h: novalang.y
	bison --defines novalang.y

lex.yy.c: novalang.l
	flex novalang.l

lex.yy.o: lex.yy.c novalang.tab.h

clean:
	rm lex.yy.* novalang.tab.* novalang 
