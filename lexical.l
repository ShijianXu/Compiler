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
struct { yylval.nd = insert("STRUCT", 0); return STRUCT; }
return { yylval.nd = insert("RETURN", 0); return RETURN; }
if { yylval.nd = insert("IF", 0); return IF; }
else { yylval.nd = insert("ELSE", 0); return ELSE; }
while { yylval.nd = insert("WHILE", 0); return WHILE; }
{type} { yylval.nd = insert("TYPE", 0); return TYPE; }
{int} { yylval.nd = insert("INT", 0); return INT; }
{float} { yylval.nd = insert("FLOAT", 0); return FLOAT; }
{id} { yylval.nd = insert("ID", 0); return ID; }
";" { yylval.nd = insert("SEMI", 0); return SEMI; }
"," { yylval.nd = insert("COMMA", 0); return COMMA; }
"=" { yylval.nd = insert("ASSIGNOP", 0); return ASSIGNOP; }
">"|"<"|">="|"<="|"=="|"!=" { yylval.nd = insert("RELOP", 0); return RELOP; }
"+" { yylval.nd = insert("PLUS", 0); return PLUS; }
"-" { yylval.nd = insert("MINUS", 0); return MINUS; }
"*" { yylval.nd = insert("STAR", 0); return STAR; }
"/" { yylval.nd = insert("DIV", 0); return DIV; }
"&&" { yylval.nd = insert("AND", 0); return AND; }
"||" { yylval.nd = insert("OR", 0); return OR; }
"." { yylval.nd = insert("DOT", 0); return DOT; }
"!" { yylval.nd = insert("NOT", 0); return NOT; }
"(" { yylval.nd = insert("LP", 0); return LP; }
")" { yylval.nd = insert("RP", 0); return RP; }
"[" { yylval.nd = insert("LB", 0); return LB; }
"]" { yylval.nd = insert("RB", 0); return RB; }
"{" { yylval.nd = insert("LC", 0); return LC; }
"}" { yylval.nd = insert("RC", 0); return RC; }
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
