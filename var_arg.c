#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tree.h"

int demo(char *ch, int a , ...)
{
	va_list argp;
	tree *para;

	va_start(argp,a);
	int count = 1;
	for(int i = 0; i < a; i++)
	{
		para = va_arg(argp, tree*);
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
	demo("Hello", 3,node1, node2, node3);
}
