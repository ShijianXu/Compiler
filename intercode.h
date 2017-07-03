#ifndef INTERCODE_H_
#define INTERCODE_H_

#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct InterCodes_* InterCodes;
typedef struct SymVar_* SymVar;
typedef struct ArrDim_* ArrDim;

struct Args_return
{
	Operand pt;
	InterCodes code;
};

struct Operand_
{
	enum { VARIABLE, TEMP, CONSTANT, ADDRESS, REF, FUNCNAME, LABEL, RELOP_OP, SIZE } kind;
	union {
		int var_no;		//addr
		int temp_no;	//ref
		int label_no;
		int value;
		char relop_sym[4];

		char name[40];
	} u;
	Operand next;
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
		CALL,
		XANDY,  //X := &Y
		XSTARY, //X := *Y
		STARXY  //*X := Y
	} kind;
	union {
		struct {Operand right, left;} assign;//xandy,xstary,starxy
		struct { Operand result, op1, op2; } binop;
		struct { Operand return_val; } return_code;
		struct { Operand op; } funcdec;
		struct { Operand re, fun; } funcall;
		struct { Operand label; } label_code;
		struct { Operand op1, op, op2, lt; } if_goto;
		struct { Operand label; } goto_;
		struct { Operand op; } read;
		struct { Operand op; } write;
		struct { Operand op; } arg;
		struct { Operand op; } param;
		struct { Operand op1, op2; } dec_;
	} u;
};

struct InterCodes_
{
	struct Code icode;
	struct InterCodes_ *prev, *next;
};

struct SymVar_
{
	char sym_name[40];
	int var_no;
	SymVar next;
};

struct ArrDim_
{
	int var_no;
	int dim;
	int dim_size[100];
	ArrDim next;
};

ArrDim ADhead;
SymVar SVhead;

struct InterCodes_ *IChead;

void init();
void dfs_(tree* root);
void writeInterCode(FILE *fp);
void printOp(Operand op, FILE *fp);
void genInterCode(tree* root, FILE* fp_ir, FILE *fp);
void insert_code(InterCodes);
int lookup_symvar(tree* root);
void insert_symvar(SymVar pt);
ArrDim lookup_arrdim(int var_no);
void insert_arrdim(ArrDim pt);

void translate_FunDec(tree* root);
Operand translate_VarDec(tree* root);
InterCodes translate_ParamDec(tree* root);
InterCodes translate_VarList(tree* root);
InterCodes translate_CompSt(tree* root);
InterCodes translate_StmtList(tree* root);
InterCodes translate_Stmt(tree* root);
InterCodes translate_Exp(tree* root, Operand place);
InterCodes translate_Cond(tree* root, Operand label_true, Operand label_false);

InterCodes translate_Dec(tree* root);
InterCodes translate_DecList(tree* root);
InterCodes translate_Def(tree* root);
InterCodes translate_DefList(tree* root);

InterCodes translate_Args(tree* root, Operand *arg_list);
InterCodes bindCode(InterCodes code1, InterCodes code2);

Operand new_label();
Operand new_temp();
Operand new_var();
Operand get_relop(tree* root);
#endif
