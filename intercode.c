#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "intercode.h"

void init()
{
	IChead=(InterCodes)malloc(sizeof(struct InterCodes_));
	
	IChead->next = IChead;
	IChead->prev = IChead;
}

void genInterCode(tree* root, FILE *fp)
{
	init();
	dfs_(root);
//	fprintf(fp, "Hello, world! %d\n", 2);
//	fclose(fp);
}

void writeInterCode(FILE *fp)
{
	InterCodes pt = IChead;
	do
	{
		pt = pt->next;
	}
	while(pt != IChead);
}

void insert_code(InterCodes code)
{
	//if()	
}

void translate_FunDec(tree* root)
{
	InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
	code->icode.kind = FUNCTION_C;
	
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCNAME;
	tree* child = root->first_child;//ID LP VarList RP
	strcpy(op->u.name, child->value);
	
	code->icode.u.funcall.op = op;

	//VarList
	insert_code(code);
}

void translate_CompSt(tree* root)
{
	
}

void dfs_(tree *root)
{
	if(strcmp(root->name, "ExtDef")==0)
	{
		tree* child = root->first_child->next_sibling;
		if(strcmp(child->name,"ExtDecList")==0)
		{
			//DO NOTHING
			//NO USE OF GLOBAL VAR
		}
		else if(strcmp(child->name,"SEMI")==0)
		{
			//感觉不需要翻译结构体
		}
		else if(strcmp(child->name,"FunDec")==0)
		{
			translate_FunDec(child);
			child=child->next_sibling;	//CompSt
			translate_CompSt(child);
		}
	}
	if(root->first_child != NULL)
		dfs_(root->first_child);
	if(root->next_sibling != NULL)
		dfs_(root->next_sibling);
}
