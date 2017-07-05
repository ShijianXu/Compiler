#ifndef MIPS_H_
#define MIPS_H_

#include "intercode.h"

struct Reg_
{
	char *name;
	int use;
};

struct Reg_ RegList[32];


void ir2mips(FILE* fp);
int getReg();
void initReg();

#endif
