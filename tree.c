#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "tree.h"

tree *insert(char *ch, int num, int lineno, ...)
{
	va_list argp;
	tree *para;
	tree *pre_sibling;

	va_start(argp, lineno);

	tree *parent = (tree *)malloc(sizeof(tree));
	parent->name = ch;
	if(lineno != -1)
		parent->line = lineno;

	if(lineno!=-1 && num==0)
		parent->terminal = 1;
	else
		parent->terminal = 0;

	if(num==0 && lineno==-1)
		parent->empty = 1;
	else
		parent->empty=0;


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
			if(lineno==-1)
			{
				parent->line = para->line;
			}
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

void treePrint(tree *root, int space)
{
	if(root->empty==0 && root->terminal==0)
	{
		for(int i=0;i<space;i++)
			printf(" ");
		printf("%s (%d)\n", root->name, root->line);
	}
	else if(root->empty==0 && root->terminal==1)
	{
		for(int i=0;i<space;i++)
			printf(" ");
		printf("%s\n", root->name);
	}

	if(root->first_child != NULL)
	{
		treePrint(root->first_child, space+2);
	}

	if(root->next_sibling != NULL)
	{
		treePrint(root->next_sibling, space);
	}
}
