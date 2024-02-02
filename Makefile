CC=gcc
CFLAGS=-O2 -Wall --debug

OBJ=novalang.tab.o lex.yy.o symbol.o ast.o

.PHONY=clean test

novalang: $(OBJ)
	$(CC) -o $@ $(OBJ)

%.o: %.c %.h

novalang.tab.c novalang.tab.h: novalang.y
	bison --defines novalang.y -Wcounterexamples

lex.yy.c: novalang.l
	flex novalang.l

lex.yy.o: lex.yy.c novalang.tab.h

test: novalang
	sh tests.sh

clean:
	rm lex.yy.* novalang.tab.* novalang 
