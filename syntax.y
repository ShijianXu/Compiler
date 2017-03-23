%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "tree.h"
	int yylex();
	int yyerror(char *msg);
	tree *root;
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
Program : ExtDefList {root = $$ = insert("Program", 1, $1);}
	;
ExtDefList : ExtDef ExtDefList {$$=insert("ExtDefList", 2, $1, $2);}
	| /* empty */ {$$=insert("ExtDefList", 0);}
	;
ExtDef : Specifier ExtDecList SEMI {$$=insert("ExtDef", 3, $1, $2, $3);}
	| Specifier SEMI {$$=insert("ExtDef", 2, $1, $2);}
	| Specifier FunDec CompSt {$$=insert("ExtDef", 3, $1, $2, $3);}
	;
ExtDecList : VarDec {$$=insert("ExtDecList", 1, $1);}
	| VarDec COMMA ExtDecList {$$=insert("ExtDecList", 3, $1, $2, $3);}
	;
/* Specifiers */
Specifier : TYPE {$$=insert("Specifier", 1, $1);}
	| StructSpecifier {$$=insert("Specifier", 1, $1);}
	;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=insert("StructSpecifier", 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag {$$=insert("StructSpecifier", 2, $1, $2);}
	;
OptTag : ID {$$=insert("OptTag", 1, $1);}
	| /* empty */ {$$=insert("OptTag", 0);}
	;
Tag : ID {$$=insert("Tag", 1, $1);}
	;
/* Declarators */
VarDec : ID {$$=insert("VarDec", 1, $1);}
	| VarDec LB INT RB {$$=insert("VarDec", 4, $1, $2, $3, $4);}
	;
FunDec : ID LP VarList RP {$$=insert("FunDec", 4, $1, $2, $3, $4);}
	| ID LP RP {$$=insert("FunDec", 3, $1, $2, $3);}
	;
VarList : ParamDec COMMA VarList {$$=insert("VarList", 3, $1, $2, $3);}
	| ParamDec {$$=insert("VarList", 1, $1);}
	;
ParamDec : Specifier VarDec {$$=insert("ParamDec", 2, $1, $2);}
	;
/* Statements */
CompSt : LC DefList StmtList RC {$$=insert("CompSt", 4, $1, $2, $3, $4);}
	;
StmtList : Stmt StmtList {$$=insert("StmtList", 2, $1, $2);}
	| /* empty */ {$$=insert("StmtList", 0);}
	;
Stmt : Exp SEMI {$$=insert("Stmt", 2, $1, $2);}
	| CompSt {$$=insert("Stmt", 1, $1);}
	| RETURN Exp SEMI {$$=insert("Stmt", 3, $1, $2, $3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=insert("Stmt", 5, $1, $2, $3, $4, $5);}
	| IF LP Exp RP Stmt ELSE Stmt {$$=insert("Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
	| WHILE LP Exp RP Stmt {$$=insert("Stmt", 5, $1, $2, $3, $4, $5);}
	;
/* Local Definitions */
DefList : Def DefList {$$=insert("DefList", 2, $1, $2);}
	| /* empty */ {$$=insert("DefList", 0);}
	;
Def : Specifier DecList SEMI {$$=insert("Def", 3, $1, $2, $3);}
	;
DecList : Dec {$$=insert("DecList", 1, $1);}
	| Dec COMMA DecList {$$=insert("DecList", 3, $1, $2, $3);}
	;
Dec : VarDec {$$=insert("Dec", 1, $1);}
	| VarDec ASSIGNOP Exp {$$=insert("Dec", 3, $1, $2, $3);}
	;
/* Expressions */
Exp : Exp ASSIGNOP Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp AND Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp OR Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp RELOP Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp PLUS Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp MINUS Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp STAR Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp DIV Exp {$$=insert("Exp", 3, $1, $2, $3);}
	| LP Exp RP {$$=insert("Exp", 3, $1, $2, $3);}
	| MINUS Exp {$$=insert("Exp", 2, $1, $2);}
	| NOT Exp {$$=insert("Exp", 2, $1, $2);}
	| ID LP Args RP {$$=insert("Exp", 4, $1, $2, $3, $4);}
	| ID LP RP {$$=insert("Exp", 3, $1, $2, $3);}
	| Exp LB Exp RB {$$=insert("Exp", 4, $1, $2, $3, $4);}
	| Exp DOT ID {$$=insert("Exp", 3, $1, $2, $3);}
	| ID {$$=insert("Exp", 1, $1);}
	| INT {$$=insert("Exp", 1, $1);}
	| FLOAT {$$=insert("Exp", 1, $1);}
	;
Args : Exp COMMA Args {$$=insert("Args", 3, $1, $2, $3);}
	| Exp {$$=insert("Args", 1, $1);}
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
	
	treePrint(root);
}
int yyerror(char* msg)
{       
	fprintf(stderr, "error line,column %d, %d: %s\n",yylloc.first_line, yylloc.first_column,msg);
}
