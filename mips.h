#ifndef MIPS_H_
#define MIPS_H_

#include "intercode.h"

void ir2mips(FILE* fp);
int getReg(Operand op);




typedef struct AsmCode_* AsmCode;
struct AsmCode_
{
	enum 
	{
		func_m,
		label_m,
		li_m,
		move_m,
		addi_m,
		add_m,
		sub_m,
		mul_m,
		div_m,
		mflo_m,
		lw_m,
		sw_m,
		j_m,
		jal_m,
		jr_m,
		beq_m,
		bne_m,
		bgt_m,
		blt_m,
		bge_m,
		ble_m 
	} mips_type;
	union
	{
		struct { char name[40]; } FUNLABEL;
		struct { char reg[5]; int cons; } LI;
		struct { char reg1[5], reg2[5]; } binop; //move, div
		struct { char reg1[5], reg2[5]; int cons; } ADDI;
		struct { char reg1[5], reg2[5], reg3[5]; } triop; //add, sub, mul
		struct { char reg1[5], reg2[5]; char labelname; } BR; //beq, bne, bgt, blt, ble
	} op;

	AsmCode next;
};

AsmCode AsmHead;

#endif
