CC=gcc
CFLAGS=-O2 -Wall --debug

OBJ=novalang.tab.o lex.yy.o symbol.o ast.o

.PHONY=clean test

novalang: $(OBJ) novalang.svg
	$(CC) -o $@ $(OBJ) -ll

%.o: %.c %.h

novalang.tab.c novalang.tab.h: novalang.y
	bison --defines novalang.y -Wcounterexamples

lex.yy.c: novalang.l
	flex novalang.l

lex.yy.o: lex.yy.c novalang.tab.h

novalang.svg: novalang.dot
	dot -Tsvg novalang.dot -o novalang.svg

test: novalang
	sh tests.sh

clean:
	rm lex.yy.* novalang.tab.* novalang.dot novalang.output novalang.svg novalang 
