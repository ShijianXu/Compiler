#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "semantic.h"

void semantic_check(tree *root)
{
	printf("checking\n");	
}

//符号表的大小确定，为0x3fff<-->16384个
unsigned hash(char *name)
{
	unsigned i;
	unsigned val = 0;
	for(;*name;++name)
	{
		val = (val<<2)+ *name;
		if(i=val & ~0x3fff)
			val = (val ^ (i>>12)) & 0x3fff;
	}
	return val;
}

void init_hash_head()
{
	int i = 0;
	for(;i<hash_size;i++)
		hash_head[i]=NULL;
}

int insert_symtable(sympt node)
{
	unsigned index = hash(node->name);
	printf("%d\n", index);

	if(hash_head[index] == NULL)
	{
		hash_head[index] = node;
		node->next = NULL;
		return 0;
	}
	else
	{
		node->next = hash_head[index];
		hash_head[index] = node;
		return 1;
	}
}
