#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "intercode.h"

void genInterCode(tree* root, FILE* fp)
{
	fprintf(fp, "Hello, world! %d\n", 2);
	fclose(fp);
}
