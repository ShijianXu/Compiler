#ifndef INTERCODE_H_
#define INTERCODE_H_

#include "semantic.h"

typedef struct Operand_* Operand;

struct Operand_
{
	enum { VARIABLE, CONSTANT, ADDRESS } kind;
	union {
		int var_no;
		int value;
		//...
	} u;
};

struct InterCode
{
	enum { ASSIGN, ADD, SUB, MUL } kind;
	union {
		struct { Operand right, left; } assign;
		struct { Operand result, op1, op2; } binop;
		//...
	} u;
};

struct InterCodes
{
	struct InterCode code;
	struct InterCodes *prev, *next;
};


void genInterCode(tree* root, FILE* fp);

#endif
