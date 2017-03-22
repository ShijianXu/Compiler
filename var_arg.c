#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"

int demo(int a , ...)
{
	va_list argp;
	tree *para;

	va_start(argp,a);
	int count = 1;
	while(1)
	{
		para = va_arg(argp, tree*);
		if(strcmp(para->name, "c")==0)
			break;
		printf("%s\n", para->name);
	}
	va_end(argp);
	return 0;
}

int main()
{
	tree *node1 = create("a");
	tree *node2 = create("b");
	tree *node3 = create("c");
	demo(1,node1, node2, node3);
}
