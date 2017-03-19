CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

parser: syntax.tab.c
	$(CC) -o parser syntax.tab.c -lfl -ly

syntax.tab.c: syntax.y lex.yy.c
	$(BISON) -d -v syntax.y

lex.yy.c: lexical.l
	$(FLEX) lexical.l

.PHONY: clean run
run:
	./parser test.c
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
