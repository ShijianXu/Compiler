#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "intercode.h"


static int labelNo= 0;
static int tempNo = 0;

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
	writeInterCode(fp);
//	fprintf(fp, "Hello, world! %d\n", 2);
//	fclose(fp);
}

void writeInterCode(FILE *fp)
{
	InterCodes pt = IChead;
	pt = pt->next;
	while(pt!=IChead)
	{
		switch(pt->icode.kind)
		{
			case FUNCTION_C:
			{
				fprintf(fp, "FUNCTION %s :\n", pt->icode.u.funcdec.op->u.name);
				break;
			}
		}
		pt=pt->next;
	}
}

void insert_code(InterCodes code)
{
	InterCodes pt = IChead;
	pt->prev->next = code;
	code->next = pt;
	code->prev = pt->prev;
	pt->prev = code;
}

void translate_FunDec(tree* root)
{
	InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
	code->icode.kind = FUNCTION_C;
	
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCNAME;
	tree* child = root->first_child;//ID LP VarList RP
	strcpy(op->u.name, child->value);
	
	code->icode.u.funcdec.op = op;
	insert_code(code);

	//VarList
	//translate_VarList();
	//TODO
}

void translate_DefList(tree* root)
{
}

Operand new_label()
{
	Operand label=(Operand)malloc(sizeof(struct Operand_));
	label->kind = LABEL;
	labelNo++;
	label->u.label_no = labelNo;
	return label;
}

Operand new_temp()
{
	Operand temp=(Operand)malloc(sizeof(struct Operand_));
	temp->kind = TEMP;
	tempNo++;
	temp->u.temp_no = tempNo;
	return temp;
}

InterCodes translate_Exp(tree* root, Operand place)
{
	if(root->child_num == 1)
	{
		//INT,ID
		tree* child = root->first_child;
		if(strcmp(child->name, "INT")==0)
		{
			Operand cons=(Operand)malloc(sizeof(struct Operand_));
			cons->kind = CONSTANT;
			cons->u.value = atoi(child->value);

			InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
			code->icode.kind = ASSIGN_C;
			code->icode.u.assign.left = place;
			code->icode.u.assign.right = cons;

			return code;
		}
	}
	else if(root->child_num == 2)
	{
		//MINUS Exp
		//NOT Exp
	}
	else if(root->child_num == 3)
	{
		tree* child = root->first_child->next_sibling;
		//Exp ASSIGNOP Exp
		if(strcmp(child->name, "ASSIGNOP")==0)
		{
		}
		//Exp RELOP Exp
		else if(strcmp(child->name, "RELOP")==0)
		{

		}
		//Exp AND Exp
		//Exp OR Exp
		//Exp PLUS Exp
		//Exp MINUS Exp
		//Exp STAR Exp
		//Exp DIV Exp
		//LP Exp RP
		//ID LP RP
		//Exp DOT ID
	}
	else if(root->child_num == 4)
	{
		//ID LP Args RP
		//Exp LB Exp RB
	}
}

Operand get_relop(tree* root)
{
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = RELOP_OP;
	strcpy(op->relop_sym, root->value);
	return op;
}

InterCodes translate_Cond(tree* root, Operand label_true, Operand label_false)
{
	tree* child = root->first_child;
	if(strcmp(child->name, "NOT")==0)
	{
		//NOT Exp
	}
	else
	{
		child = child->next_sibling;
		if(strcmp(child->name,"RELOP")==0)
		{
			//Exp RELOP Exp
			Operand t1 = new_temp();
			Operand t2 = new_temp();
			Operand op = get_relop(child);

			child = root->first_child;//Exp1
			InterCodes code1=translate_Exp(child, t1);
			
			child = child->next_sibling->next_sibling;//Exp2
			InterCodes code2=translate_Exp(child, t2);
			
			InterCodes code3=(InterCodes)malloc(sizeof(struct InterCodes_));
			code3->icode.kind = IF_GOTO;
			code3->icode.u.if_goto.op1 = t1;
			code3->icode.u.if_goto.op = op;
			code3->icode.u.if_goto.op2 = t2;
			code3->icode.u.if_goto.lt = label_true;

			InterCodes code4=(InterCodes)malloc(sizeof(struct InterCodes_));
			code4->icode.kind = GOTO;
			code4->icode.u.goto_.label = label_false;

			//return(bindCode(bindCode(code1, code2),bindCode(code3,code4)));

		}
		else if(strcmp(child->name,"AND")==0)
		{
			//Exp AND Exp
		}
		else if(strcmp(child->name,"OR")==0)
		{
			//Exp OR Exp
		}
		else
		{
			//other case
			//???
		}
	}
}

InterCodes translate_Stmt(tree* root)
{
	if(root->child_num == 1)
	{
		//CompSt
	}
	else if(root->child_num == 2)
	{
		//Exp SEMI
	}
	else if(root->child_num == 3)
	{
		//RETURN Exp SEMI
	}
	else if(root->child_num == 5)
	{
		//IF    LP Exp RP Stmt
		//WHILE LP Exp RP Stmt
		tree* child = root->first_child;
		if(strcmp(child->name, "IF")==0)
		{
			Operand label1 = new_label();
			Operand label2 = new_label();
			
			child = child->next_sibling->next_sibling;//Exp
			InterCodes code1=translate_Cond(child, label1, label2);
			//...
			child = child->next_sibling->next_sibling;//Stmt
			InterCodes code2=translate_Stmt(child);

			InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode1->icode.kind = LABEL_C;
			labelcode1->icode.u.label_code.label = label1;

			InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode2->icode.kind = LABEL_C;
			labelcode2->icode.u.label_code.label = label2;

			//TODO
		}
		else
		{

		}
	}
	else if(root->child_num == 7)
	{
		//IF LP Exp RP Stmt ELSE Stmt
		//Operand label1 = new_label();
		//Operand label2 = new_label();
		//Operand label3 = new_label();
		
		//InterCodes code1 = translate
	}
}

void translate_StmtList(tree* root)
{
	tree* child = root->first_child;	//Stmt
	translate_Stmt(child);


	child = child->next_sibling;	//StmtList
	translate_StmtList(child);
}

void translate_CompSt(tree* root)
{
	tree* child = root->first_child->next_sibling;//DefList
	translate_DefList(child);

	child = child->next_sibling;	//StmtList
	translate_StmtList(child);
}

void dfs_(tree *root)
{
	if(strcmp(root->name, "ExtDef")==0)
	{
		tree* child = root->first_child->next_sibling;
		if(strcmp(child->name,"ExtDecList")==0)
		{
			//全局变量定义
			//不需要操作
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
