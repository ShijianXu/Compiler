CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99

parser: syntax.tab.c tree.c
	$(CC) -o parser tree.c semantic.c syntax.tab.c -lfl -ly

syntax.tab.c: syntax.y lex.yy.c
	$(BISON) -d -t -v syntax.y

lex.yy.c: lexical.l
	$(FLEX) lexical.l

.PHONY: clean run
run:
	./parser test.cmm
clean:
	rm -f parser lex.yy.c syntax.tab.c syntax.tab.h syntax.output
