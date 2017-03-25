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
type "int"|"float"
id {letter}({letter}|{digit})*
hex 0[xX]({digit}|[a-fA-F])+
oct 0[0-7]+
int {hex}|{oct}|(([1-9]{digit}*)|0)
float ({digit}+\.{digit}+)|({digit}*\.{digit}*[Ee][+-]?{digit}+)
%%

{ws} {/*no action and no return*/}
\n { yycolumn = 1 ;}
struct { yylval.nd = insert("STRUCT", 0, "none", yylineno); return STRUCT; }
return { yylval.nd = insert("RETURN", 0, "none", yylineno); return RETURN; }
if { yylval.nd = insert("IF", 0, "none", yylineno); return IF; }
else { yylval.nd = insert("ELSE", 0, "none", yylineno); return ELSE; }
while { yylval.nd = insert("WHILE", 0, "none", yylineno); return WHILE; }
{type} { yylval.nd = insert("TYPE", 0, yytext, yylineno); return TYPE; }
{int} { yylval.nd = insert("INT", 0, yytext, yylineno); return INT; }
{float} { yylval.nd = insert("FLOAT", 0, yytext, yylineno); return FLOAT; }
{id} { yylval.nd = insert("ID", 0, yytext, yylineno); return ID; }
";" { yylval.nd = insert("SEMI", 0, "none", yylineno); return SEMI; }
"," { yylval.nd = insert("COMMA", 0, "none", yylineno); return COMMA; }
"=" { yylval.nd = insert("ASSIGNOP", 0, "none", yylineno); return ASSIGNOP; }
">"|"<"|">="|"<="|"=="|"!=" { yylval.nd = insert("RELOP", 0, "none",yylineno); return RELOP; }
"+" { yylval.nd = insert("PLUS", 0, "none", yylineno); return PLUS; }
"-" { yylval.nd = insert("MINUS", 0, "none", yylineno); return MINUS; }
"*" { yylval.nd = insert("STAR", 0, "none", yylineno); return STAR; }
"/" { yylval.nd = insert("DIV", 0, "none", yylineno); return DIV; }
"&&" { yylval.nd = insert("AND", 0, "none", yylineno); return AND; }
"||" { yylval.nd = insert("OR", 0, "none", yylineno); return OR; }
"." { yylval.nd = insert("DOT", 0, "none", yylineno); return DOT; }
"!" { yylval.nd = insert("NOT", 0, "none", yylineno); return NOT; }
"(" { yylval.nd = insert("LP", 0, "none", yylineno); return LP; }
")" { yylval.nd = insert("RP", 0, "none", yylineno); return RP; }
"[" { yylval.nd = insert("LB", 0, "none", yylineno); return LB; }
"]" { yylval.nd = insert("RB", 0, "none", yylineno); return RB; }
"{" { yylval.nd = insert("LC", 0, "none", yylineno); return LC; }
"}" { yylval.nd = insert("RC", 0, "none", yylineno); return RC; }
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
