CC = gcc
FLEX = flex
BISON = bison
CFLAGS = -std=c99
OBJ = scanner

scanner: lex.yy.c
	@$(CC) lex.yy.c -lfl -o $(OBJ)
lex.yy.c: lexical.l
	@$(FLEX) lexical.l

.PHONY: clean run
run:
	@./$(OBJ) test.c
clean:
	@rm *.yy.c scanner
