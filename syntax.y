%{
	#include <stdio.h>
	int yylex();
	int yyerror(char *msg);
%}
%locations
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

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%%
/* High-level Definitions */
Program : ExtDefList
	;
ExtDefList : ExtDef ExtDefList
	| /* empty */
	;
ExtDef : Specifier ExtDecList SEMI
	| Specifier SEMI
	| Specifier FunDec CompSt
	;
ExtDecList : VarDec
	| VarDec COMMA ExtDecList
	;
/* Specifiers */
Specifier : TYPE
	| StructSpecifier
	;
StructSpecifier : STRUCT OptTag LC DefList RC
	| STRUCT Tag
	;
OptTag : ID
	| /* empty */
	;
Tag : ID
	;
/* Declarators */
VarDec : ID
	| VarDec LB INT RB
	;
FunDec : ID LP VarList RP
	| ID LP RP
	;
VarList : ParamDec COMMA VarList
	| ParamDec
	;
ParamDec : Specifier VarDec
	;
/* Statements */
CompSt : LC DefList StmtList RC
	;
StmtList : Stmt StmtList
	| /* empty */
	;
Stmt : Exp SEMI
	| CompSt
	| RETURN Exp SEMI
	| IF LP Exp RP Stmt
	| IF LP Exp RP Stmt ELSE Stmt
	| WHILE LP Exp RP Stmt
	;
/* Local Definitions */
DefList : Def DefList
	| /* empty */
	;
Def : Specifier DecList SEMI
	;
DecList : Dec
	| Dec COMMA DecList
	;
Dec : VarDec
	| VarDec ASSIGNOP Exp
	;
/* Expressions */
Exp : Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID
	| INT
	| FLOAT
	;
Args : Exp COMMA Args
	| Exp
	;

%%
#include "lex.yy.c"

extern FILE* yyin;
int main(int argc, char** argv)
{   
	if (argc <= 1) return 1;
	
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyin = f;
	yyrestart(f);
	do{
		/*yydebug = 1;*/
		yyparse();
	}while(!feof(yyin));
}
int yyerror(char* msg)
{       
	fprintf(stderr, "error line %d: %s\n",yylloc.first_line, msg);
}
