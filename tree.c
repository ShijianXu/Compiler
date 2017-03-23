#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "tree.h"

tree *insert(char *ch, int num, ...)
{
	va_list argp;
	tree *para;
	tree *pre_sibling;

	va_start(argp, num);

	tree *parent = (tree *)malloc(sizeof(tree));
	parent->name = ch;
	
	if(num == 0)
	{
		parent->first_child = NULL;
		parent->next_sibling = NULL;
		return parent;
	}

	for(int i = 0; i < num; i ++)
	{
		para = va_arg(argp, tree*);
		if(i==0)	//first child
		{
			parent->first_child = para;
			pre_sibling = para;
		}
		else //a sibling, connected to its former sibling
		{
			pre_sibling->next_sibling = para;
			pre_sibling = para;
		}
	}
	return parent;
}

void treePrint(tree *root)
{
	printf("%s\n", root->name);

	if(root->first_child != NULL)
	{
		treePrint(root->first_child);
	}

	if(root->next_sibling != NULL)
	{
		treePrint(root->next_sibling);
	}
}
