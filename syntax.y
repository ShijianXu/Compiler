%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "tree.h"
	int yylex();
	int yyerror(char *msg);
%}
%locations
/* delcared types */
%union {
	tree *nd;
	double type_double;
}
/* declared tokens */
%token <nd> INT FLOAT
%token <nd> ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND
%token <nd> OR DOT NOT TYPE LP RP LB RB LC RC
%token <nd> STRUCT RETURN IF ELSE WHILE

%type <nd> Program ExtDefList ExtDef ExtDecList Specifier 
%type <nd> StructSpecifier OptTag Tag VarDec FunDec VarList 
%type <nd> ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

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
Program : ExtDefList {printf("Program\n");}
	;
ExtDefList : ExtDef ExtDefList {printf("ExtDefList\n");}
	| /* empty */
	;
ExtDef : Specifier ExtDecList SEMI {printf("ExtDef\n");}
	| Specifier SEMI {printf("ExtDef\n");}
	| Specifier FunDec CompSt {printf("ExtDef\n");}
	;
ExtDecList : VarDec {printf("ExtDecList\n");}
	| VarDec COMMA ExtDecList {printf("ExtDecList\n");}
	;
/* Specifiers */
Specifier : TYPE {printf("Specifier\n");}
	| StructSpecifier {printf("Specifier\n");}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {printf("StructSpecifier\n");}
	| STRUCT Tag {printf("StructSpecifier\n");}
	;
OptTag : ID {printf("OptTag\n");}
	| /* empty */
	;
Tag : ID {printf("Tag\n");}
	;
/* Declarators */
VarDec : ID {printf("VarDec\n");}
	| VarDec LB INT RB {printf("VarDec\n");}
	;
FunDec : ID LP VarList RP {printf("FunDec\n");}
	| ID LP RP {printf("FunDec\n");}
	;
VarList : ParamDec COMMA VarList {printf("VarList\n");}
	| ParamDec {printf("VarList\n");}
	;
ParamDec : Specifier VarDec {printf("ParamDec\n");}
	;
/* Statements */
CompSt : LC DefList StmtList RC {printf("CompSt\n");}
	;
StmtList : Stmt StmtList {printf("StmtList\n");}
	| /* empty */
	;
Stmt : Exp SEMI {printf("Stmt\n");}
	| CompSt {printf("Stmt\n");}
	| RETURN Exp SEMI {printf("Stmt\n");}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {printf("Stmt\n");}
	| IF LP Exp RP Stmt ELSE Stmt {printf("Stmt\n");}
	| WHILE LP Exp RP Stmt {printf("Stmt\n");}
	;
/* Local Definitions */
DefList : Def DefList {printf("DefList\n");}
	| /* empty */
	;
Def : Specifier DecList SEMI {printf("Def\n");}
	;
DecList : Dec {printf("DecList\n");}
	| Dec COMMA DecList {printf("DecList\n");}
	;
Dec : VarDec {printf("Dec\n");}
	| VarDec ASSIGNOP Exp {printf("Dec\n");}
	;
/* Expressions */
Exp : Exp ASSIGNOP Exp {printf("Exp\n");}
	| Exp AND Exp {printf("Exp\n");}
	| Exp OR Exp {printf("Exp\n");}
	| Exp RELOP Exp {printf("Exp\n");}
	| Exp PLUS Exp {printf("Exp\n");}
	| Exp MINUS Exp {printf("Exp\n");}
	| Exp STAR Exp {printf("Exp\n");}
	| Exp DIV Exp {printf("Exp\n");}
	| LP Exp RP {printf("Exp\n");}
	| MINUS Exp {printf("Exp\n");}
	| NOT Exp {printf("Exp\n");}
	| ID LP Args RP {printf("Exp\n");}
	| ID LP RP {printf("Exp\n");}
	| Exp LB Exp RB {printf("Exp\n");}
	| Exp LB error RB {printf("Exp\n");}
	| Exp DOT ID {printf("Exp\n");}
	| ID {printf("Exp\n");}
	| INT {printf("Exp\n");}
	| FLOAT {printf("Exp\n");}
	;
Args : Exp COMMA Args {printf("Args\n");}
	| Exp {printf("Args\n");}
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
	fprintf(stderr, "error line,column %d, %d: %s\n",yylloc.first_line, yylloc.first_column,msg);
}
