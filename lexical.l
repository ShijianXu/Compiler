%{
	#include "syntax.tab.h"
%}

%option yylineno
delim [ \t\n]
ws {delim}+
letter [_a-zA-Z]
digit [0-9]
type "int"|"float"|"char"
id {letter}({letter}|{digit})*
hex 0[xX]({digit}|[a-fA-F])+
oct 0[0-7]+
int {hex}|{oct}|(([1-9]{digit}*)|0)
float ({digit}+\.{digit}+)|({digit}*\.{digit}*[Ee][+-]?{digit}+)
%%

{ws} {/*no action and no return*/}
struct { printf("STRUCT\n"); return STRUCT; }
return { printf("RETURN\n"); return RETURN; }
if { printf("IF\n"); return IF; }
else { printf("ELSE\n"); return ELSE; }
while { printf("WHILE\n"); return WHILE; }
for { printf("FOR\n"); }
{type} { printf("TYPE:%s\n", yytext); return TYPE; }
{int} { printf("INT:%s\n", yytext); return INT; }
{float} { printf("FLOAT:%s\n", yytext); return FLOAT; }
{id} { printf("ID:%s\n", yytext); return ID; }
";" { printf("SEMI\n"); return SEMI; }
"," { printf("COMMA\n"); return COMMA; }
"=" { printf("ASSIGN\n"); return ASSIGNOP; }
">"|"<"|">="|"<="|"=="|"!=" { printf("RELOP\n"); return RELOP; }
"+" { printf("PLUS\n"); return PLUS; }
"-" { printf("SUB\n"); return MINUS; }
"*" { printf("MUL\n"); return STAR; }
"/" { printf("DIV\n"); return DIV; }
"&&" { printf("AND\n"); return AND; }
"||" { printf("OR\n"); return OR; }
"." { printf("DOT\n"); return DOT; }
"!" { printf("NOT\n"); return NOT; }
"(" { printf("LP\n"); return LP; }
")" { printf("RP\n"); return RP; }
"[" { printf("LB\n"); return LB; }
"]" { printf("RB\n"); return RB; }
"{" { printf("LC\n"); return LC; }
"}" { printf("RC\n"); return RC; }
"//" {char c=input();
	while (c!='\n') c=input();
}
\"[^\"]*\" {
	if (yytext[yyleng-2]=='\\') {
		yyless(yyleng - 1);
		yymore();
	} else {
		/* process the string literal */
	}
}
.  {
	printf("Error type A at Line %d: Mysterious characters \"%s\".\n",yylineno, yytext);
}

%%

/*int main(int argc, char** argv)
{
	if (argc>1)
	{
		FILE* f = fopen(argv[1],"r");
		if(!f)
		{
			perror(argv[1]);
			return 1;
		}
	}
	//yylex();
	yyrestart(f);
	yyparse();
	return 0;
}*/
