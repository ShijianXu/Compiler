#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "mips.h"
#include <assert.h>

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

void load_w(FILE* fp, int regno, Operand op)
{
	fprintf(fp, "  lw $%s, %d($fp)\n", RegList[regno].name, op->offset);
}

Operand funop = NULL;
static int argCount = 0;
static int argFirst = 1;

static int paramCount = 0;
static int paramFirst = 1;
static int paramIndex = 0;

static int over4 = 0;


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
				argFirst = 1;
				paramFirst = 1;
				Operand op = pt->icode.u.funcdec.op;
				funop = op;
				fprintf(fp, "\n%s:\n", op->u.name);
					
					fprintf(fp, "  addi $sp, $sp, -8\n");
					fprintf(fp, "  sw $ra, 8($sp)\n");
					fprintf(fp, "  sw $fp, 4($sp)\n");
					fprintf(fp, "  move $fp, $sp\n");
				
				fprintf(fp, "  addi $sp, $sp, %d\n", op->funsize);
				break;
			}
			case LABEL_C:
			{
				argFirst = 1;
				Operand op = pt->icode.u.label_code.label;
				fprintf(fp, "label%d:\n", op->u.label_no);
				break;
			}
			case ASSIGN_C:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.assign.left;
				Operand op2 = pt->icode.u.assign.right;
				int r1, r2;
				if(op1->kind == REF && op2->kind == REF)
				{
					r1 = getReg();
					r2 = getReg();
					load_w(fp, r1, op1);
					load_w(fp, r2, op2);

					int rt = getReg();
					fprintf(fp, "  lw $%s, 0($%s)\n", RegList[rt].name, RegList[r2].name);
					
					fprintf(fp, "  sw $%s, 0($%s)\n", RegList[rt].name, RegList[r1].name);
					RegList[r1].use = 0;
					RegList[r2].use = 0;
					RegList[rt].use = 0;
				}
				else if(op1->kind == REF)
				{
					r1 = getReg();
					r2 = getReg();
					load_w(fp, r1, op1);
					load_w(fp, r2, op2);
					fprintf(fp, "  sw $%s, 0($%s)\n", RegList[r2].name, RegList[r1].name);
					RegList[r1].use = 0;
					RegList[r2].use = 0;
				}
				else if(op2->kind == REF)
				{
					r1 = getReg();
					r2 = getReg();
					load_w(fp, r2, op2);
					fprintf(fp, "  lw $%s, 0($%s)\n", RegList[r1].name, RegList[r2].name);
					store(fp, r1, op1);
					RegList[r2].use = 0;
				}
				else if(op2->kind == CONSTANT)
				{
					r1 = getReg();
					//op2为const,op1为普通var
					fprintf(fp, "  li $%s, %d\n", RegList[r1].name, op2->u.value);
					store(fp, r1, op1);
				}
				else
				{
					//两个操作数均为普通的var/temp
					r1 = getReg();
					r2 = getReg();
					load_w(fp, r2, op2);
					fprintf(fp, "  move $%s, $%s\n", RegList[r1].name, RegList[r2].name);
					store(fp, r1, op1);
					store(fp, r2, op2);
				}
				break;
			}
			case ADD_C:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.binop.result;
				int r1 = getReg(op1);
				Operand op2 = pt->icode.u.binop.op1;
				int r2;
				Operand op3 = pt->icode.u.binop.op2;
				int r3 = getReg(op3);
				if(op2->kind == ADDRESS)
				{
					r2 = getReg();
					fprintf(fp, "  la $%s, %d($fp)\n", RegList[r2].name, op2->offset);
					load_w(fp, r3, op3);
				}
				else
				{
					r2 = getReg();
					load_w(fp, r2, op2);
					load_w(fp, r3, op3);
				}
				fprintf(fp, "  add $%s $%s, $%s\n", RegList[r1].name,RegList[r2].name,RegList[r3].name);
				store(fp, r1, op1);
				RegList[r2].use = 0;
				store(fp, r3, op3);
				break;
			}
			case SUB_C:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.binop.result;
				int r1 = getReg(op1);
				Operand op2 = pt->icode.u.binop.op1;
				int r2 = getReg(op2);
				Operand op3 = pt->icode.u.binop.op2;
				int r3 = getReg(op3);
				load_w(fp, r2, op2);
				load_w(fp, r3, op3);
				fprintf(fp, "  sub $%s $%s, $%s\n", RegList[r1].name,RegList[r2].name,RegList[r3].name);
				store(fp, r1, op1);
				store(fp, r2, op2);
				store(fp, r3, op3);
				break;
			}
			case MUL_C:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.binop.result;
				int r1 = getReg(op1);
				
				Operand op2 = pt->icode.u.binop.op1;
				Operand op3 = pt->icode.u.binop.op2;
	
				int r2, r3;
				r2 = getReg();
				load_w(fp, r2, op2);
				if(op3->kind == CONSTANT)
				{
					r3 = getReg();
					fprintf(fp, "  li $%s, %d\n", RegList[r3].name, op3->u.value);
				}
				else
				{
					r3 = getReg();
					load_w(fp, r3, op3);
				}
				fprintf(fp, "  mul $%s $%s, $%s\n", RegList[r1].name,RegList[r2].name,RegList[r3].name);
				store(fp, r1, op1);
				store(fp, r2, op2);
				if(op3->kind != CONSTANT)
					store(fp, r3, op3);
				break;
			}
			case DIV_C:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.binop.result;
				int r1 = getReg(op1);
				Operand op2 = pt->icode.u.binop.op1;
				int r2 = getReg(op2);
				Operand op3 = pt->icode.u.binop.op2;
				int r3 = getReg(op3);
				load_w(fp, r2, op2);
				load_w(fp, r3, op3);
				fprintf(fp, "  div $%s, $%s\n", RegList[r2].name, RegList[r3].name);
				fprintf(fp, "  mflo $%s\n", RegList[r1].name);
				store(fp, r1, op1);
				store(fp, r2, op2);
				store(fp, r3, op3);
				break;
			}
			case RETURN_C:
			{
				argFirst = 1;
				Operand op = pt->icode.u.return_code.return_val;
				int r = getReg(op);
				load_w(fp, r, op);
				fprintf(fp, "  move $v0, $%s\n", RegList[r].name);
				fprintf(fp, "  lw $ra, 8($fp)\n");
				fprintf(fp, "  lw $fp, 4($fp)\n");
				fprintf(fp, "  addi $sp, $sp, %d\n", (4*over4)+8 - funop->funsize);
				fprintf(fp, "  jr $ra\n");
				break;
			}
			case IF_GOTO:
			{
				argFirst = 1;
				Operand op1 = pt->icode.u.if_goto.op1;
				Operand op = pt->icode.u.if_goto.op;
				Operand op2 = pt->icode.u.if_goto.op2;
				Operand lt = pt->icode.u.if_goto.lt;
				
				int r1 = getReg();
				int r2 = getReg();
				load_w(fp, r1, op1);
				load_w(fp, r2, op2);

				assert(op->kind == RELOP_OP);
				if(strcmp(op->u.relop_sym,"<")==0)
				{
					fprintf(fp, "  blt $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
				else if(strcmp(op->u.relop_sym, "==")==0)
				{
					fprintf(fp, "  beq $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
				else if(strcmp(op->u.relop_sym, "!=")==0)
				{
					fprintf(fp, "  bne $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
				else if(strcmp(op->u.relop_sym, ">")==0)
				{
					fprintf(fp, "  bgt $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
				else if(strcmp(op->u.relop_sym, ">=")==0)
				{
					fprintf(fp, "  bge $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
				else if(strcmp(op->u.relop_sym, "<=")==0)
				{
					fprintf(fp, "  ble $%s, $%s, label%d\n", RegList[r1].name, RegList[r2].name, lt->u.label_no);
				}
			
				store(fp, r1, op1);
				store(fp, r2, op2);
				break;
			}
			case GOTO:
			{
				argFirst = 1;
				Operand op = pt->icode.u.label_code.label;
				fprintf(fp, "  j label%d\n",op->u.label_no);
				break;
			}
			case READ:
			{
				argFirst = 1;
				fprintf(fp, "  jal read\n");
				Operand op = pt->icode.u.read.op;
				int r = getReg();
				fprintf(fp, "  move $%s, $v0\n", RegList[r].name);
				store(fp, r, op);
				break;
			}
			case WRITE:
			{
				argFirst = 1;
				Operand op = pt->icode.u.write.op;
				if(op->kind != REF)
				{
					int r = getReg();
					load_w(fp, r, op);
					RegList[r].use = 0;
					fprintf(fp, "  move $a0, $%s\n", RegList[r].name);
				}
				else
				{
					int r = getReg();
					load_w(fp, r, op);
					int rt = getReg();
					fprintf(fp, "  lw $%s, 0($%s)\n", RegList[rt].name, RegList[r].name);
					RegList[r].use = 0;
					RegList[rt].use = 0;
					fprintf(fp, "  move $a0, $%s\n", RegList[rt].name);
				}
				fprintf(fp, "  jal write\n");
				break;
			}
			case ARG:
			{
				if(argFirst)
				{
					argCount = 0;
					argCount ++;
					InterCodes ic = pt->next;
					while(ic->icode.kind == ARG)
					{
						argCount++;
						ic = ic->next;
					}
					int t = argCount-4;
					over4 = t>0 ? t : 0;

					fprintf(fp, "  addi $sp, $sp, %d\n", 0-over4*4);
					argFirst = 0;
				}
				argCount --;
				if(argCount > 3)
				{
					Operand op = pt->icode.u.arg.op;
					int r = getReg();
					load_w(fp, r, op);

					if(op->kind == REF)
					{
						int rt = getReg();
						fprintf(fp, "  lw $%s, 0($%s)\n", RegList[rt].name, RegList[r].name);
						fprintf(fp, "  sw $%s, %d($sp)\n", RegList[rt].name, (argCount-3)*4);
						RegList[r].use = 0;
						RegList[rt].use = 0;
					}
					else
					{
						fprintf(fp, "  sw $%s, %d($sp)\n", RegList[r].name, (argCount-3)*4);
						RegList[r].use = 0;

					}
				}
				else
				{
					//放入寄存器a0~a3
					Operand op = pt->icode.u.arg.op;
					int r = getReg();
					load_w(fp, r, op);
					
					if(op->kind == REF)
					{
						int rt = getReg();
						fprintf(fp, "  lw $%s, 0($%s)\n", RegList[rt].name, RegList[r].name);
						fprintf(fp, "  move $a%d, $%s\n", argCount, RegList[rt].name);
						RegList[r].use = 0;
						RegList[rt].use = 0;
					}
					else
					{
						fprintf(fp, "  move $a%d, $%s\n", argCount, RegList[r].name);
						RegList[r].use = 0;

					}
				}

				break;
			}
			case PARAM:
			{
				argFirst = 1;
				if(paramFirst)
				{
					paramCount = 0;
					paramCount ++;
					paramIndex = 0;
					InterCodes ic = pt->next;
					while(ic->icode.kind == PARAM)
					{
						paramCount++;
						ic = ic->next;
					}
					paramFirst = 0;
				}
				
				if(paramIndex >3)
				{
					Operand op = pt->icode.u.param.op;
					int r = getReg();
					fprintf(fp, "  lw $%s, %d($fp)\n", RegList[r].name, 8+(paramIndex-3)*4);
					store(fp, r, op);
					paramIndex++;
				}
				else
				{
					Operand op = pt->icode.u.param.op;
					int r = getReg();
					fprintf(fp, "  move $%s, $a%d\n", RegList[r].name, paramIndex);
					store(fp, r, op);
					paramIndex++;
				}
				break;
			}
			case DEC:
			{
				//DEC 数组定义不需要产生代码
				//只要预先分配地址空间就够了
				argFirst = 1;
				break;
			}
			case CALL:
			{
				argFirst = 1;
				Operand op = pt->icode.u.funcall.re;
				int r = getReg();
				Operand fun = pt->icode.u.funcall.fun;
				fprintf(fp, "  jal %s\n", fun->u.name);
				fprintf(fp, "  move $%s, $v0\n", RegList[r].name);
				store(fp, r, op);
				break;
			}
		}
		pt = pt->next;
	}
}
