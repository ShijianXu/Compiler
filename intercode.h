#ifndef INTERCODE_H_
#define INTERCODE_H_

#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;

struct Operand_
{
	enum { VARIABLE, TEMP, CONSTANT, ADDRESS, FUNCNAME, LABEL } kind;
	union {
		int var_no;
		int temp_no;
		int label_no;
		int value;
		char name[40];
		//...
	} u;
};

struct Code
{
	enum { 
		ASSIGN_C, 
		ADD_C, 
		SUB_C, 
		MUL_C, 
		DIV_C, 
		RETURN_C, 
		IF_GOTO, 
		GOTO, 
		LABEL_C, 
		READ, 
		WRITE, 
		FUNCTION_C, 
		ARG, 
		PARAM, 
		DEC,
		CALL
	} kind;
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop;
		struct { Operand op; } funcdec;
		struct { Operand op; } funcall;
		struct { Operand label; } label_code;
		//...
	} u;
};

struct InterCodes_
{
	struct Code icode;
	struct InterCodes_ *prev, *next;
};

struct InterCodes_ *IChead;

void init();
void dfs_(tree* root);
void writeInterCode(FILE *fp);
void genInterCode(tree* root, FILE *fp);

void insert_code(InterCodes);
void translate_FunDec(tree* root);
void translate_CompSt(tree* root);
void translate_StmtList(tree* child);
InterCodes translate_Stmt(tree* child);
void translate_DefList(tree* child);

Operand new_label();
Operand new_temp();
#endif
