CC=gcc
CFLAGS=-O2 -Wall --debug

OBJ=novalang.tab.o lex.yy.o ast/symbol/symbol.o ast/ast.o

.PHONY=clean test

novalang: $(OBJ)
	$(CC) -o $@ $(OBJ) -Iast -Isymbol

%.o: %.c %.h

novalang.tab.c novalang.tab.h: novalang.y
	bison --defines novalang.y -Wcounterexamples

lex.yy.c: novalang.l
	flex novalang.l

lex.yy.o: lex.yy.c novalang.tab.h

test: novalang
	sh tests.sh

clean:
	rm ast/ast.o ast/symbol/symbol.o lex.yy.* novalang.tab.* novalang
