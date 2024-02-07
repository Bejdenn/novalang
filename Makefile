CC=gcc
CFLAGS=-O3 -Wall --debug

OBJ=novalang.tab.o lex.yy.o ast/symbol/symbol.o ast/symbol/functions.o ast/node.o ast/declaration.o ast/val.o ast/op.o ast/assignment.o ast/fn.o ast/ref.o ast/block.o ast/flow.o ast/ast.o

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
	rm ast/*.o ast/symbol/*.o lex.yy.* novalang.tab.* novalang
