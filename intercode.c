#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "intercode.h"


static int labelNo= 0;
static int tempNo = 0;
static int varNo = 0;

void init()
{
	IChead=(InterCodes)malloc(sizeof(struct InterCodes_));
	
	IChead->next = IChead;
	IChead->prev = IChead;

	SVhead = NULL;
}

void genInterCode(tree* root, FILE *fp)
{
	init();
	dfs_(root);
	writeInterCode(fp);
//	fprintf(fp, "Hello, world! %d\n", 2);
//	fclose(fp);
}

void printOp(Operand op, FILE *fp)
{
	switch(op->kind)
	{
		case VARIABLE:
		{
			fprintf(fp, "v%d", op->u.var_no);
			break;
		}
		case TEMP:
		{
			fprintf(fp, "t%d", op->u.temp_no);
			break;
		}
		case CONSTANT:
		{
			fprintf(fp, "#%d", op->u.value);
			break;
		}
		case FUNCNAME:
		{
			fprintf(fp, "%s", op->u.name);
			break;
		}
		case LABEL:
		{
			fprintf(fp, "label%d", op->u.label_no);
			break;
		}
		case RELOP_OP:
		{
			fprintf(fp, "%s", op->u.relop_sym);
			break;
		}
	}
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
				fprintf(fp, "FUNCTION ");
				Operand op = pt->icode.u.funcdec.op;
				printOp(op,fp);
				fprintf(fp, " :\n");
				break;
			}
			case ASSIGN_C:
			{
				Operand op = pt->icode.u.assign.left;
				printOp(op,fp);
				fprintf(fp, " := ");
				op=pt->icode.u.assign.right;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case ADD_C:
			{
				Operand op = pt->icode.u.binop.result;
				printOp(op,fp);
				fprintf(fp," := ");
				op=pt->icode.u.binop.op1;
				printOp(op,fp);
				fprintf(fp, " + ");
				op=pt->icode.u.binop.op2;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case SUB_C:
			{
				Operand op = pt->icode.u.binop.result;
				printOp(op,fp);
				fprintf(fp," := ");
				op=pt->icode.u.binop.op1;
				printOp(op,fp);
				fprintf(fp, " - ");
				op=pt->icode.u.binop.op2;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case MUL_C:
			{
				Operand op = pt->icode.u.binop.result;
				printOp(op,fp);
				fprintf(fp," := ");
				op=pt->icode.u.binop.op1;
				printOp(op,fp);
				fprintf(fp, " * ");
				op=pt->icode.u.binop.op2;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case DIV_C:
			{
				Operand op = pt->icode.u.binop.result;
				printOp(op,fp);
				fprintf(fp," := ");
				op=pt->icode.u.binop.op1;
				printOp(op,fp);
				fprintf(fp, " / ");
				op=pt->icode.u.binop.op2;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case RETURN_C:
			{
				fprintf(fp, "RETURN ");
				Operand op = pt->icode.u.return_code.return_val;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case IF_GOTO:
			{
				fprintf(fp, "IF ");
				Operand op = pt->icode.u.if_goto.op1;
				printOp(op,fp);
				fprintf(fp," ");
				op = pt->icode.u.if_goto.op;
				printOp(op,fp);
				fprintf(fp," ");
				op = pt->icode.u.if_goto.op2;
				printOp(op,fp);
				fprintf(fp," GOTO ");
				op = pt->icode.u.if_goto.lt;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case GOTO:
			{
				fprintf(fp, "GOTO ");
				Operand op = pt->icode.u.goto_.label;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case LABEL_C:
			{
				fprintf(fp,"LABEL ");
				Operand op = pt->icode.u.label_code.label;
				printOp(op,fp);
				fprintf(fp," :\n");
				break;
			}
			case READ:
			{
				fprintf(fp,"READ ");
				Operand op = pt->icode.u.read.op;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case WRITE:
			{
				fprintf(fp,"WRITE ");
				Operand op = pt->icode.u.write.op;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case ARG:
			{
				fprintf(fp,"ARG ");
				Operand op = pt->icode.u.arg.op;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case PARAM:
			{
				fprintf(fp,"PARAM ");
				Operand op = pt->icode.u.param.op;
				printOp(op,fp);
				fprintf(fp,"\n");
				break;
			}
			case DEC:
			{
				break;
			}
			case CALL:
			{
				Operand op = pt->icode.u.funcall.re;
				printOp(op,fp);
				fprintf(fp," := CALL ");
				op = pt->icode.u.funcall.fun;
				printOp(op,fp);
				fprintf(fp, "\n");
				break;
			}
		}
		pt=pt->next;
	}
}

void insert_code(InterCodes code2)
{
	assert(code2!=NULL);

	InterCodes code1 = IChead;

	code1->prev->next = code2;
	code2->prev->next = code1;
	InterCodes pt = code2->prev;
	code2->prev = code1->prev;
	code1->prev = pt;
}

Operand translate_VarDec(tree* root)
{
	//ID
	if(root->child_num == 1)
	{
		Operand var = new_var();
		tree* child = root->first_child;
		int var_no = lookup_symvar(child);
		if(var_no != -1)
		{
			var->u.var_no = var_no;
		}
		else
		{
			SymVar sv=(SymVar)malloc(sizeof(struct SymVar_));
			strcpy(sv->sym_name, child->value);
			sv->var_no = var->u.var_no;
			sv->next = NULL;
			insert_symvar(sv);
		}
		return var;
	}
	else
	{
		//VarDec LB INT RB
		//TODO
	}
}

InterCodes translate_ParamDec(tree* root)
{
	//Specifier VarDec

	tree* child = root->first_child->next_sibling;
	Operand op = translate_VarDec(child);
	InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
	code->icode.kind = PARAM;
	code->prev = code;
	code->next = code;
	code->icode.u.param.op = op;

	return code;
}

InterCodes translate_VarList(tree* root)
{
	//ParamDec COMMA VarList;
	//ParamDec
	if(root->child_num == 3)
	{
		tree* child = root->first_child;
		InterCodes code1 = translate_ParamDec(child);
		child = child->next_sibling->next_sibling;
		InterCodes code2 = translate_VarList(child);
		return bindCode(code1,code2);
	}
	else
	{
		tree* child = root->first_child;
		return translate_ParamDec(child);
	}
}

void translate_FunDec(tree* root)
{
	InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
	code1->icode.kind = FUNCTION_C;
	
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = FUNCNAME;
	op->next = NULL;
	tree* child = root->first_child;//ID LP VarList RP
	strcpy(op->u.name, child->value);
	
	code1->icode.u.funcdec.op = op;
	code1->prev = code1;
	code1->next = code1;

	child = child->next_sibling->next_sibling;
	if(strcmp(child->name,"VarList")==0)
	{
		InterCodes code2 = translate_VarList(child);

		InterCodes code = bindCode(code1,code2);
		insert_code(code);
	}
	else
		insert_code(code1);
}

Operand new_label()
{
	Operand label=(Operand)malloc(sizeof(struct Operand_));
	label->kind = LABEL;
	label->next = NULL;
	labelNo++;
	label->u.label_no = labelNo;
	return label;
}

Operand new_temp()
{
	Operand temp=(Operand)malloc(sizeof(struct Operand_));
	temp->kind = TEMP;
	temp->next = NULL;
	tempNo++;
	temp->u.temp_no = tempNo;
	return temp;
}

Operand new_var()
{
	Operand var =(Operand)malloc(sizeof(struct Operand_));
	var->kind = VARIABLE;
	var->next = NULL;
	varNo++;
	var->u.var_no = varNo;
	return var;
}

int lookup_symvar(tree* root)
{
	//root == ID
	SymVar pt = SVhead;
	while(pt!=NULL)
	{
		if(strcmp(root->value, pt->sym_name)==0)
			return pt->var_no;
		pt = pt->next;
	}
	return -1;
}

void insert_symvar(SymVar pt)
{
	if(SVhead == NULL)
	{
		SVhead = pt;
	}
	else
	{
		pt->next = SVhead;
		SVhead = pt;
	}
}

InterCodes translate_Exp(tree* root, Operand place)
{
	if(root->child_num == 1)
	{
		//INT,ID
		tree* child = root->first_child;
		if(strcmp(child->name, "INT")==0)
		{
			printf("INT\n");
			Operand cons=(Operand)malloc(sizeof(struct Operand_));
			cons->kind = CONSTANT;
			cons->next = NULL;
			cons->u.value = atoi(child->value);

			InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
			code->icode.kind = ASSIGN_C;
			code->icode.u.assign.left = place;
			code->icode.u.assign.right = cons;
			code->prev = code;
			code->next = code;

			return code;
		}
		else if(strcmp(child->name, "ID")==0)
		{
			printf("ID\n");
			Operand var = new_var();
			int var_no = lookup_symvar(child);
			if(var_no != -1)
			{
				var->u.var_no = var_no;
			}
			else
			{
				SymVar sv=(SymVar)malloc(sizeof(struct SymVar_));
				strcpy(sv->sym_name, child->value);
				sv->var_no = var->u.var_no;
				sv->next = NULL;
				insert_symvar(sv);
			}

			InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
			code->icode.kind = ASSIGN_C;
			code->icode.u.assign.left = place;
			code->icode.u.assign.right = var;
			code->prev = code;
			code->next = code;
			return code;
		}
	}
	else if(root->child_num == 2)
	{
		//MINUS Exp
		tree* child = root->first_child;
		if(strcmp(child->name, "MINUS")==0)
		{
			Operand t1 = new_temp();
			child = child->next_sibling;
			InterCodes code1 = translate_Exp(child, t1);
			InterCodes code2=(InterCodes)malloc(sizeof(struct InterCodes_));
			code2->icode.kind = SUB_C;
			code2->next = code2;
			code2->prev = code2;
			
			Operand op = (Operand)malloc(sizeof(struct Operand_));
			op->kind = CONSTANT;
			op->next = NULL;
			op->u.value = 0;
			code2->icode.u.binop.op1 = op;
			code2->icode.u.binop.op2 = t1;
			code2->icode.u.binop.result = place;
			return bindCode(code1, code2);
		}
		else
		{
			//Exp-->NOT Exp
			Operand label1 = new_label();
			Operand label2 = new_label();
			InterCodes code0=(InterCodes)malloc(sizeof(struct InterCodes_));
			code0->icode.kind = ASSIGN_C;
			code0->prev = code0;
			code0->next = code0;
			Operand op1=(Operand)malloc(sizeof(struct Operand_));
			op1->kind = CONSTANT;
			op1->next = NULL;
			op1->u.value = 0;
			code0->icode.u.assign.left = place;
			code0->icode.u.assign.right = op1;

			InterCodes code1=translate_Cond(root, label1, label2);

			InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode1->icode.kind = LABEL_C;
			labelcode1->icode.u.label_code.label=label1;
			labelcode1->prev = labelcode1;
			labelcode1->next = labelcode1;

			InterCodes ascode=(InterCodes)malloc(sizeof(struct InterCodes_));
			ascode->icode.kind = ASSIGN_C;
			ascode->icode.u.assign.left = place;
			ascode->prev = ascode;
			ascode->next = ascode;
			Operand asop =(Operand)malloc(sizeof(struct Operand_));
			asop->kind = CONSTANT;
			asop->next = NULL;
			asop->u.value = 1;
			ascode->icode.u.assign.right = asop;

			InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode2->icode.kind = LABEL_C;
			labelcode2->icode.u.label_code.label=label2;
			labelcode2->prev = labelcode2;
			labelcode2->next = labelcode2;

			return bindCode(bindCode(bindCode(code0,code1), bindCode(labelcode1,ascode)), labelcode2);
		}
	}
	else if(root->child_num == 3)
	{
		tree* child = root->first_child->next_sibling;
		//Exp ASSIGNOP Exp
		if(strcmp(child->name, "ASSIGNOP")==0)
		{
			printf("Exp ASSIGNOP Exp\n");
			child=root->first_child;	//Exp1
			assert(place!=NULL);
			InterCodes code = translate_Exp(child,place);

			Operand t1 = new_temp();
			child = child->next_sibling->next_sibling;//Exp2
			InterCodes code1 = translate_Exp(child,t1);
		
			InterCodes code2=(InterCodes)malloc(sizeof(struct InterCodes_));
			code2->icode.kind = ASSIGN_C;
			code2->icode.u.assign.left=code->icode.u.assign.right;
			code2->icode.u.assign.right = t1;
			code2->prev = code2;
			code2->next = code2;

			//code3 = code
			return bindCode(code1,bindCode(code2, code));
		}
		else if(strcmp(child->name, "PLUS")==0 || strcmp(child->name,"MINUS")==0 || strcmp(child->name, "STAR")==0 || strcmp(child->name, "DIV")==0 )
		{
			//root-->Exp PLUS Exp
					//Exp MINUS Exp
					//Exp STAR Exp
					//Exp DIV Exp
			Operand t1 = new_temp();
			Operand t2 = new_temp();
			child = root->first_child;
			InterCodes code1=translate_Exp(child, t1);
			child = child->next_sibling->next_sibling;
			InterCodes code2=translate_Exp(child, t2);

			InterCodes code3=(InterCodes)malloc(sizeof(struct InterCodes_));
			code3->prev = code3;
			code3->next = code3;
			code3->icode.u.binop.op1 = t1;
			code3->icode.u.binop.op2 = t2;
			code3->icode.u.binop.result = place;
			child=root->first_child->next_sibling;
			if(strcmp(child->name,"PLUS")==0)
				code3->icode.kind = ADD_C;
			else if(strcmp(child->name,"MINUS")==0)
				code3->icode.kind = SUB_C;
			else if(strcmp(child->name,"STAR")==0)
				code3->icode.kind = MUL_C;
			else if(strcmp(child->name,"DIV")==0)
				code3->icode.kind = DIV_C;

			return bindCode(code1,bindCode(code2,code3));
		}
		else if(strcmp(child->name,"AND")==0 || strcmp(child->name,"OR")==0 || strcmp(child->name,"RELOP")==0)
		{
			//root-->Exp AND Exp
					//Exp OR Exp
					//Exp RELOP Exp
			Operand label1 = new_label();
			Operand label2 = new_label();
			InterCodes code0=(InterCodes)malloc(sizeof(struct InterCodes_));
			code0->icode.kind = ASSIGN_C;
			code0->prev = code0;
			code0->next = code0;
			Operand op1=(Operand)malloc(sizeof(struct Operand_));
			op1->kind = CONSTANT;
			op1->next = NULL;
			op1->u.value = 0;
			code0->icode.u.assign.left = place;
			code0->icode.u.assign.right = op1;

			InterCodes code1=translate_Cond(root, label1, label2);

			InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode1->icode.kind = LABEL_C;
			labelcode1->icode.u.label_code.label=label1;
			labelcode1->prev = labelcode1;
			labelcode1->next = labelcode1;

			InterCodes ascode=(InterCodes)malloc(sizeof(struct InterCodes_));
			ascode->icode.kind = ASSIGN_C;
			ascode->icode.u.assign.left = place;
			ascode->prev = ascode;
			ascode->next = ascode;
			Operand asop =(Operand)malloc(sizeof(struct Operand_));
			asop->kind = CONSTANT;
			asop->next = NULL;
			asop->u.value = 1;
			ascode->icode.u.assign.right = asop;

			InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode2->icode.kind = LABEL_C;
			labelcode2->icode.u.label_code.label=label2;
			labelcode2->prev = labelcode2;
			labelcode2->next = labelcode2;

			return bindCode(bindCode(bindCode(code0,code1), bindCode(labelcode1,ascode)), labelcode2);
		}
		else if(strcmp(child->name, "LP")==0)
		{
			//ID LP RP 无参函数调用
			child = root->first_child;
			InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
			code->prev = code;
			code->next = code;

			if(strcmp(child->value, "read")==0)
			{
				code->icode.kind = READ;
				code->icode.u.read.op = place;
			}
			else
			{
				Operand op = (Operand)malloc(sizeof(struct Operand_));
				op->kind = FUNCNAME;
				op->next = NULL;

				code->icode.kind = CALL;
				strcpy(op->u.name, child->value);
				code->icode.u.funcall.fun = op;
				code->icode.u.funcall.re = place;
			}
			return code;
		}
		else if(strcmp(child->name,"Exp")==0)
		{
			//LP Exp RP
			return translate_Exp(child, place);
		}
		else
		{
			//Exp DOT ID
			//TODO
		}
	}
	else if(root->child_num == 4)
	{
		//ID LP Args RP
		tree* child = root->first_child;
		if(strcmp(child->name,"ID")==0)
		{
			Operand arg_list=NULL;

			child = child->next_sibling->next_sibling;
			InterCodes code1 = translate_Args(child, &arg_list);
		
			assert(arg_list != NULL);

			child = root->first_child;
			if(strcmp(child->value, "write")==0)
			{
				InterCodes code=(InterCodes)malloc(sizeof(struct InterCodes_));
				code->prev = code;
				code->next = code;
				code->icode.kind = WRITE;

				assert(arg_list!=NULL);

				code->icode.u.write.op = arg_list;
				return bindCode(code1,code);
			}
			else
			{
				Operand pt = arg_list;
				InterCodes code2;
			
				while(pt!=NULL)
				{
					InterCodes argcode=(InterCodes)malloc(sizeof(struct InterCodes_));
					argcode->prev = argcode;
					argcode->next = argcode;
					argcode->icode.kind = ARG;
					argcode->icode.u.arg.op = pt;
	
					if(pt == arg_list)
						code2 = argcode;
					else
						code2 = bindCode(code2, argcode);
					
					pt=pt->next;
				}

				InterCodes callcode=(InterCodes)malloc(sizeof(struct InterCodes_));
				callcode->prev = callcode;
				callcode->next = callcode;
				callcode->icode.kind = CALL;
				Operand op=(Operand)malloc(sizeof(struct Operand_));
				op->kind = FUNCNAME;
				op->next = NULL;
				child = root->first_child;
				strcpy(op->u.name, child->value);
				callcode->icode.u.funcall.fun = op;
				callcode->icode.u.funcall.re = place;
				return bindCode(bindCode(code1,code2),callcode);
			}
		}
		//Exp LB Exp RB
		//TODO
	}
}

Operand get_relop(tree* root)
{
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->kind = RELOP_OP;
	op->next = NULL;
	strcpy(op->u.relop_sym, root->value);
	return op;
}

InterCodes bindCode(InterCodes code1, InterCodes code2)
{
	assert(code1!=NULL && code2!=NULL);
	code1->prev->next = code2;
	code2->prev->next = code1;

	InterCodes pt = code2->prev;
	
	code2->prev = code1->prev;
	code1->prev = pt;

	return code1;
}

InterCodes translate_Args(tree* root, Operand *arg_list)
{
	//Exp
	//Exp COMMA Args
	if(root->child_num == 1)
	{
		Operand t1 = new_temp();
		tree* child = root->first_child;
		InterCodes code1=translate_Exp(child, t1);
	
		if(*arg_list == NULL)
		{
			*arg_list = t1;
		}
		else
		{
			t1->next = *arg_list;
			*arg_list = t1;
		}
		return code1;
	}
	else
	{
		Operand t1 = new_temp();
		tree* child = root->first_child;
		InterCodes code1 = translate_Exp(child, t1);
		
		if(*arg_list == NULL)
		{
			*arg_list = t1;
		}
		else
		{
			t1->next = *arg_list;
			*arg_list = t1;
		}
		child = child->next_sibling->next_sibling;
		InterCodes code2 = translate_Args(child, arg_list);
		return bindCode(code1, code2);
	}
}

InterCodes translate_Cond(tree* root, Operand label_true, Operand label_false)
{
	tree* child = root->first_child;
	if(strcmp(child->name, "NOT")==0)
	{
		child=child->next_sibling;
		return translate_Cond(child, label_false, label_true);
	}
	else
	{
		child = child->next_sibling;
		if(strcmp(child->name,"RELOP")==0)
		{
			printf("Exp RELOP Exp\n");
			//Exp RELOP Exp
			Operand t1 = new_temp();
			Operand t2 = new_temp();
			Operand op = get_relop(child);
			printf("relop_sym: %s\n", op->u.name);		
	
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
			code3->prev = code3;
			code3->next = code3;

			InterCodes code4=(InterCodes)malloc(sizeof(struct InterCodes_));
			code4->icode.kind = GOTO;
			code4->icode.u.goto_.label = label_false;
			code4->prev = code4;
			code4->next = code4;
		
			return bindCode(bindCode(code1, code2),bindCode(code3,code4));
		}
		else if(strcmp(child->name,"AND")==0)
		{
			//Exp AND Exp
			Operand label1 = new_label();
			child = root->first_child;
			InterCodes code1=translate_Cond(child, label1, label_false);
			child = child->next_sibling->next_sibling;
			InterCodes code2=translate_Cond(child, label_true, label_false);
			InterCodes labelcode=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode->prev = labelcode;
			labelcode->next = labelcode;
			labelcode->icode.kind = LABEL_C;
			labelcode->icode.u.label_code.label = label1;

			return bindCode(bindCode(code1, labelcode), code2);
		}
		else if(strcmp(child->name,"OR")==0)
		{
			//Exp OR Exp
			Operand label1 = new_label();
			child = root->first_child;
			InterCodes code1=translate_Cond(child, label_true, label1);
			child = child->next_sibling->next_sibling;
			InterCodes code2=translate_Cond(child, label_true, label_false);
			InterCodes labelcode=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode->prev = labelcode;
			labelcode->next = labelcode;
			labelcode->icode.kind = LABEL_C;
			labelcode->icode.u.label_code.label = label1;

			return bindCode(bindCode(code1, labelcode), code2);
		}
		else
		{
			//other case
			//???
			Operand t1 = new_temp();
			InterCodes code1 = translate_Exp(root,t1);
			InterCodes code2=(InterCodes)malloc(sizeof(struct InterCodes_));
			code2->prev = code2;
			code2->next = code2;
			code2->icode.kind = IF_GOTO;
			code2->icode.u.if_goto.op1 = t1;
			code2->icode.u.if_goto.lt = label_true;

			Operand relop=(Operand)malloc(sizeof(struct Operand_));
			relop->kind = RELOP_OP;
			relop->next = NULL;
			strcpy(relop->u.relop_sym,"!=");
			code2->icode.u.if_goto.op = relop;

			Operand op2 = (Operand)malloc(sizeof(struct Operand_));
			op2->kind = CONSTANT;
			op2->next = NULL;
			op2->u.value = 0;
			code2->icode.u.if_goto.op2 = op2;

			InterCodes labelcode=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode->prev = labelcode;
			labelcode->next = labelcode;
			labelcode->icode.kind = GOTO;
			labelcode->icode.u.goto_.label = label_true;

			return bindCode(bindCode(code1,code2), labelcode);
		}
	}
}

InterCodes translate_Stmt(tree* root)
{
	if(root->child_num == 1)
	{
		//CompSt
		tree* child = root->first_child;
		return translate_CompSt(child);
	}
	else if(root->child_num == 2)
	{
		//Exp SEMI
		Operand t1 = new_temp();
		printf("Exp SEMI\n");
		tree* child = root->first_child;	//Exp
		return translate_Exp(child, t1);
	}
	else if(root->child_num == 3)
	{
		printf("RETURN Exp SEMI\n");
		//RETURN Exp SEMI
		Operand t1 = new_temp();
		tree* child = root->first_child->next_sibling;//Exp
		InterCodes code1=translate_Exp(child, t1);
		InterCodes code2=(InterCodes)malloc(sizeof(struct InterCodes_));
		code2->icode.kind = RETURN_C;
		code2->icode.u.return_code.return_val=t1;
		code2->next = code2;
		code2->prev = code2;

		InterCodes ic = bindCode(code1,code2);
		return ic;
	}
	else if(root->child_num == 5)
	{
		//IF    LP Exp RP Stmt
		tree* child = root->first_child;
		if(strcmp(child->name, "IF")==0)
		{
			printf("IF LP Exp RP Stmt\n");

			Operand label1 = new_label();
			Operand label2 = new_label();
			
			child = child->next_sibling->next_sibling;//Exp
			assert(strcmp(child->name,"Exp")==0);
			InterCodes code1=translate_Cond(child, label1, label2);
		
			child = child->next_sibling->next_sibling;//Stmt
			assert(strcmp(child->name,"Stmt")==0);
			InterCodes code2=translate_Stmt(child);

			InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode1->icode.kind = LABEL_C;
			labelcode1->icode.u.label_code.label = label1;
			labelcode1->prev = labelcode1;
			labelcode1->next = labelcode1;

			InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode2->icode.kind = LABEL_C;
			labelcode2->icode.u.label_code.label = label2;
			labelcode2->prev = labelcode2;
			labelcode2->next = labelcode2;

			return bindCode(bindCode(code1, labelcode1),bindCode(code2,labelcode2));
		}
		else
		{
			//WHILE LP Exp RP Stmt
			printf("WHILE LP Exp RP Stmt\n");
			Operand label1 = new_label();
			Operand label2 = new_label();
			Operand label3 = new_label();
			tree* child = root->first_child->next_sibling->next_sibling;
			InterCodes code1 = translate_Cond(child, label2,label3);
			child = child->next_sibling->next_sibling;
			InterCodes code2 = translate_Stmt(child);

			InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode1->icode.kind = LABEL_C;
			labelcode1->icode.u.label_code.label = label1;
			labelcode1->prev = labelcode1;
			labelcode1->next = labelcode1;

			InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode2->icode.kind = LABEL_C;
			labelcode2->icode.u.label_code.label = label2;
			labelcode2->prev = labelcode2;
			labelcode2->next = labelcode2;

			InterCodes gotocode=(InterCodes)malloc(sizeof(struct InterCodes_));
			gotocode->icode.kind = GOTO;
			gotocode->icode.u.goto_.label = label1;
			gotocode->prev = gotocode;
			gotocode->next = gotocode;

			InterCodes labelcode3=(InterCodes)malloc(sizeof(struct InterCodes_));
			labelcode3->icode.kind = LABEL_C;
			labelcode3->icode.u.label_code.label = label3;
			labelcode3->prev = labelcode3;
			labelcode3->next = labelcode3;

			return bindCode(bindCode(bindCode(labelcode1, code1), bindCode(labelcode2, code2)),bindCode(gotocode, labelcode3));
		}
	}
	else if(root->child_num == 7)
	{
		//IF LP Exp RP Stmt ELSE Stmt
		Operand label1 = new_label();
		Operand label2 = new_label();
		Operand label3 = new_label();
		
		tree* child = root->first_child->next_sibling->next_sibling;
		InterCodes code1 = translate_Cond(child, label1,label2);
		child = child->next_sibling->next_sibling;
		InterCodes code2 = translate_Stmt(child);
		child = child->next_sibling->next_sibling;
		InterCodes code3 = translate_Stmt(child);

		InterCodes labelcode1=(InterCodes)malloc(sizeof(struct InterCodes_));
		labelcode1->icode.kind = LABEL_C;
		labelcode1->icode.u.label_code.label = label1;
		labelcode1->prev = labelcode1;
		labelcode1->next = labelcode1;

		InterCodes gotocode=(InterCodes)malloc(sizeof(struct InterCodes_));
		gotocode->icode.kind = GOTO;
		gotocode->icode.u.goto_.label = label3;
		gotocode->prev = gotocode;
		gotocode->next = gotocode;

		InterCodes labelcode2=(InterCodes)malloc(sizeof(struct InterCodes_));
		labelcode2->icode.kind = LABEL_C;
		labelcode2->icode.u.label_code.label = label2;
		labelcode2->prev = labelcode2;
		labelcode2->next = labelcode2;

		InterCodes labelcode3=(InterCodes)malloc(sizeof(struct InterCodes_));
		labelcode3->icode.kind = LABEL_C;
		labelcode3->icode.u.label_code.label = label3;
		labelcode3->prev = labelcode3;
		labelcode3->next = labelcode3;

		return bindCode(bindCode(bindCode(bindCode(code1,labelcode1), bindCode(code2,gotocode)), bindCode(labelcode2, code3)), labelcode3);
	}
}

InterCodes translate_Dec(tree* root)
{
	if(root->child_num == 1)
	{
		//VarDec
		return NULL;
	}
	else
	{
		//VarDec ASSIGNOP Exp
		tree* child = root->first_child;
		Operand op1 = translate_VarDec(child);
		InterCodes code1=(InterCodes)malloc(sizeof(struct InterCodes_));
		code1->next = code1;
		code1->prev = code1;
		code1->icode.kind = ASSIGN_C;
		code1->icode.u.assign.left = op1;

		Operand op2 = new_temp();
		code1->icode.u.assign.right = op2;
		child = child->next_sibling->next_sibling;
		InterCodes code2=translate_Exp(child, op2);
		return bindCode(code2,code1);
	}
}

InterCodes translate_DecList(tree* root)
{
	if(root->child_num == 1)
	{
		//Dec
		tree* child = root->first_child;
		return translate_Dec(child);
		//NULL
	}
	else
	{
		//Dec COMMA DecList
		tree* child = root->first_child;
		InterCodes code1 = translate_Dec(child);
		child = child->next_sibling->next_sibling;
		InterCodes code2 = translate_DecList(child);

		if(code1!=NULL && code2!=NULL)
			return bindCode(code1, code2);
		else if(code1!=NULL)
			return code1;
		else if(code2!=NULL)
			return code2;
		else
			return NULL;
	}
}

InterCodes translate_Def(tree* root)
{
	//Specifier DecList SEMI
	tree* child = root->first_child->next_sibling;
	return translate_DecList(child);
	//NULL
}

InterCodes translate_DefList(tree* root)
{
	if(root->child_num == 0)
		return NULL;
	else
	{
		//Def DefList
		tree* child = root->first_child;
		InterCodes code1 = translate_Def(child);
		
		child = child->next_sibling;
		InterCodes code2 = translate_DefList(child);
	
		if(code1 != NULL && code2 != NULL)
			return bindCode(code1, code2);
		else if(code2 != NULL)
			return code2;
		else if(code1 != NULL)
			return code1;
		else
			return NULL;
	}
}

InterCodes translate_StmtList(tree* root)
{
	if(root->child_num == 0)
		return NULL;
	//Stmt StmtList | epsilon
	
	tree* child = root->first_child;	//Stmt
	assert(strcmp(child->name, "Stmt")==0);
	InterCodes code1 = translate_Stmt(child);

	child = child->next_sibling;	 //StmtList
	assert(strcmp(child->name, "StmtList")==0);
	InterCodes code2 = translate_StmtList(child);

	if(code2 == NULL)
		return code1;
	else
		return bindCode(code1, code2);
}

InterCodes translate_CompSt(tree* root)
{
	printf("CompSt\n");
	tree* child = root->first_child->next_sibling;//DefList
	InterCodes code1 = translate_DefList(child);

	child = child->next_sibling;	//StmtList
	InterCodes code2 = translate_StmtList(child);

	assert(code2!=NULL);

	if(code1==NULL)
		return code2;
	else
		return bindCode(code1, code2);
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
			InterCodes code = translate_CompSt(child);
			insert_code(code);
		}
	}
	if(root->first_child != NULL)
		dfs_(root->first_child);
	if(root->next_sibling != NULL)
		dfs_(root->next_sibling);
}
