#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "mips.h"

void genMIPScode(FILE* fp)
{
	fprintf(fp, ".data\n");
	fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(fp, "_ret: .asciiz \"\\n\"\n");
	fprintf(fp, ".globl main\n");
	fprintf(fp, ".text\n");
	fprintf(fp, "read:\n");
	fprintf(fp, "  li $v0, 4\n");
	fprintf(fp, "  la $a0, _prompt\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  li $v0, 5\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  jr $ra\n");
	fprintf(fp, "\n");
	fprintf(fp, "write:\n");
	fprintf(fp, "  li $v0, 1\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  li $v0, 4\n");
	fprintf(fp, "  la $a0, _ret\n");
	fprintf(fp, "  syscall\n");
	fprintf(fp, "  move $v0, $0\n");
	fprintf(fp, "  jr $ra\n\n");

	initReg();
	ir2mips(fp);
}

void initReg()
{
	RegList[0].name = "$0";
	RegList[1].name = "at";
	RegList[2].name = "v0";
	RegList[3].name = "v1";
	RegList[4].name = "a0";
	RegList[5].name = "a1";
	RegList[6].name = "a2";
	RegList[7].name = "a3";
	
	RegList[8].name = "t0";
	RegList[9].name = "t1";
	RegList[10].name = "t2";
	RegList[11].name = "t3";
	RegList[12].name = "t4";
	RegList[13].name = "t5";
	RegList[14].name = "t6";
	RegList[15].name = "t7";
	RegList[16].name = "s0";
	RegList[17].name = "s1";
	RegList[18].name = "s2"; 
	RegList[19].name = "s3";
	RegList[20].name = "s4";
	RegList[21].name = "s5";
	RegList[22].name = "s6";
	RegList[23].name = "s7";
	RegList[24].name = "t8";
	RegList[25].name = "t9";
	
	RegList[26].name = "k0";
	RegList[27].name = "k1";
	RegList[28].name = "gp";
	RegList[29].name = "sp";
	RegList[30].name = "fp";
	RegList[31].name = "ra";
	int i = 0;
	for(;i<32;i++)
	{
		RegList[i].use = 0;
	}
}

int getReg()
{
	int i;
	for(i = 8 ;i <= 25; i++)
	{
		if(RegList[i].use == 0)
		{
			RegList[i].use = 1;
			return i;
		}
	}
	printf("error!");
	return -1;
}

void store(FILE* fp, int regno, Operand op)
{
	fprintf(fp, "  sw $%s, %d($fp)\n", RegList[regno].name, op->offset);
	RegList[regno].use = 0;
}

Operand funop = NULL;

void ir2mips(FILE* fp)
{
	InterCodes pt = IChead;
	pt = pt->next;
	while(pt!=IChead)
	{
		switch(pt->icode.kind)
		{
			case FUNCTION_C:
			{
				Operand op = pt->icode.u.funcdec.op;
				funop = op;
				fprintf(fp, "%s:\n", op->u.name);
					
					fprintf(fp, "  addi $sp, $sp, -8\n");
					fprintf(fp, "  sw $ra, 8($sp)\n");
					fprintf(fp, "  sw $fp, 4($sp)\n");
					fprintf(fp, "  move $fp, $sp\n");
				
				fprintf(fp, "  addi $sp, $sp, %d\n", op->funsize);
				break;
			}
			case LABEL_C:
			{
				Operand op = pt->icode.u.label_code.label;
				fprintf(fp, "label%d:\n", op->u.label_no);
				break;
			}
			case ASSIGN_C:
			{
				Operand op1 = pt->icode.u.assign.left;
				int r1 = getReg();
				Operand op2 = pt->icode.u.assign.right;
				if(op2->kind == CONSTANT)
				{
					fprintf(fp, "  li $%s, %d\n", RegList[r1].name, op2->u.value);
					store(fp, r1, op1);
				}
				else
				{
					int r2 = getReg();
					fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r2].name, op2->offset);
					fprintf(fp, "  move $%s, $%s\n", RegList[r1].name, RegList[r2].name);
					store(fp, r1, op1);
					store(fp, r2, op2);
				}

				fprintf(fp, "ass\n");
				break;
			}
			case ADD_C:
			{
				Operand op1 = pt->icode.u.binop.result;
				int r1 = getReg(op1);
				Operand op2 = pt->icode.u.binop.op1;
				int r2 = getReg(op2);
				Operand op3 = pt->icode.u.binop.op2;
				int r3 = getReg(op3);
				fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r2].name, op2->offset);
				fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r3].name, op3->offset);
				fprintf(fp, "  add $%s $%s, $%s\n", RegList[r1].name,RegList[r2].name,RegList[r3].name);
				store(fp, r1, op1);
				store(fp, r2, op2);
				store(fp, r3, op3);

				fprintf(fp, "add\n");
				break;
			}
			case SUB_C:
			{
				
				break;
			}
			case MUL_C:
			{
				break;
			}
			case DIV_C:
			{
				break;
			}
			case RETURN_C:
			{
				Operand op = pt->icode.u.return_code.return_val;
				int r = getReg(op);
				fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r].name, op->offset);
				fprintf(fp, "  move $v0, $%s\n", RegList[r].name);
				fprintf(fp, "  lw $ra, 8($fp)\n");
				fprintf(fp, "  lw $fp, 4($fp)\n");
				fprintf(fp, "  addi $sp, $sp, %d\n", 0-funop->funsize);
				fprintf(fp, "  jr $ra\n");

				fprintf(fp, "ret\n");
				break;
			}
			case IF_GOTO:
			{
				break;
			}
			case GOTO:
			{
				break;
			}
			case READ:
			{
				fprintf(fp, "  jal read\n");
				Operand op = pt->icode.u.read.op;
				int r = getReg(op);
				fprintf(fp, "  move $%s, $v0\n", RegList[r].name);
				store(fp, r, op);

				fprintf(fp, "read\n");
				break;
			}
			case WRITE:
			{
				Operand op = pt->icode.u.write.op;
				int r = getReg(op);
				fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r].name, op->offset);
				fprintf(fp, "  move $a0, $%s\n", RegList[r].name);
				fprintf(fp, "  jal write\n");

				fprintf(fp, "write\n");
				break;
			}
			case ARG:
			{
				break;
			}
			case PARAM:
			{
				break;
			}
			case DEC:
			{
				break;
			}
			case CALL:
			{
				break;
			}
		}
		pt = pt->next;
	}
}
