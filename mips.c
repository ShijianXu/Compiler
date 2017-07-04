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
	
	ir2mips(fp);
}

static int count = 0;
int getReg(Operand op)
{
	count++;
	return count;
}

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
				fprintf(fp, "%s:\n", op->u.name);
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
				Operand opl = pt->icode.u.assign.left;
				int r1 = getReg(opl);
				Operand opr = pt->icode.u.assign.right;
				if(opr->kind == CONSTANT)
				{
					fprintf(fp, "  li $t%d, %d\n", r1, opr->u.value);
				}
				else
				{
					int r2 = getReg(opr);
					fprintf(fp, "  move $t%d, $t%d\n", r1, r2);
				}
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
				fprintf(fp, "  add $t%d, $t%d, $t%d\n", r1,r2,r3);
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
				break;
			}
			case WRITE:
			{
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
