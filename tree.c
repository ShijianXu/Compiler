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

	tree *child = (tree *)malloc(sizeof(tree));
	child->name = ch;
	
	//printf("Parent node: %s\n", child->name);
	if(num == 0)
	{
		child->first_child = NULL;
		child->next_sibling = NULL;
		return child;
	}

	for(int i = 0; i < num; i ++)
	{
		para = va_arg(argp, tree*);
		if(i==0)	//first child
		{
			child->first_child = para;
			pre_sibling = para;
		}
		else //a sibling, connected to its former sibling
		{
			//printf("%s ", pre_sibling->name);
			pre_sibling->next_sibling = para;
			pre_sibling = para;
		}
	}
	//printf("\n");
	return child;
}

tree *create(char *ch)
{
	tree *root=(tree *)malloc(sizeof(tree));
	root->name = ch;
	return root;
}

void treePrint(tree *root)
{
	printf("%s\n", root->name);

	if(root->first_child != NULL)
	{
		treePrint(root->first_child);
	}

	tree *sibling = root->next_sibling;
	while(sibling != NULL)
	{
		treePrint(sibling);
		sibling = sibling->next_sibling;
	}
}
