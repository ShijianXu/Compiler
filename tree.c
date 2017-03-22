#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

tree *insert(char *ch, tree *parent, tree *pre_sibling)
{
	tree *child = (tree *)malloc(sizeof(tree));
	child->name = ch;
	if(parent!=NULL)
		parent->first_child = child;
	if(pre_sibling!=NULL)
		pre_sibling->next_sibling = child;

	child->first_child = NULL;
	child->next_sibling = NULL;
	
	return child;
}

tree *create(char *ch)
{
	tree *root=(tree *)malloc(sizeof(tree));
	root->name = ch;
	return root;
}
