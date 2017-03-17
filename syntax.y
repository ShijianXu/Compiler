%{
	#include <stdio.h>
	#include "lex.yy.c"
%}

/* declared tokens */
%token INT
%token FLOAT
%token ID
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token DOT
%token NOT
%token TYPE
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE

%%
Calc : /* empty */
	| Exp { printf("= %d\n", $1); }
	;
Exp : Factor
	| Exp ADD Factor { $$ = $1 + $3; }
	| Exp SUB Factor { $$ = $1 - $3; }
	;
Factor : Term
	| Factor MUL Term { $$ = $1 * $3; }
	| Factor DIV Term { $$ = $1 / $3; }
	;
Term : INT
	;

%%
#inlucde "lex.yy.c"
int main()
{       
	yyparse();
}
yyerror(char* msg)
{       
	fprintf(stderr, "error: %s\n", msg);
}
