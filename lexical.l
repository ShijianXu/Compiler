%{
	#include "syntax.tab.h"
	int yycolumn = 1;
	#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}

%option yylineno
delim [ \t]
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
\n { yycolumn = 1 ;}
struct { return STRUCT; }
return { return RETURN; }
if { return IF; }
else { return ELSE; }
while { return WHILE; }
{type} { return TYPE; }
{int} { return INT; }
{float} { return FLOAT; }
{id} { return ID; }
";" { return SEMI; }
"," { return COMMA; }
"=" { return ASSIGNOP; }
">"|"<"|">="|"<="|"=="|"!=" { return RELOP; }
"+" { return PLUS; }
"-" { return MINUS; }
"*" { return STAR; }
"/" { return DIV; }
"&&" { return AND; }
"||" { return OR; }
"." { return DOT; }
"!" { return NOT; }
"(" { return LP; }
")" { return RP; }
"[" { return LB; }
"]" { return RB; }
"{" { return LC; }
"}" { return RC; }
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
