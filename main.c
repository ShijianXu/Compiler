#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

int main()
{
	init_hash_head();
	sympt a = (sympt)malloc(sizeof(struct SymTableNode));
	sympt b = (sympt)malloc(sizeof(struct SymTableNode));
	sympt c = (sympt)malloc(sizeof(struct SymTableNode));

	char str2[10]="Compiler";
	char str3[10]="Compiler";
	char str1[10]="Compiler";
	a->name=str1;
	b->name=str2;
	c->name=str3;

	int a1 = insert_symtable(a);
	int b1 = insert_symtable(b);
	int c1 = insert_symtable(c);
	printf("%d %d %d\n", a1,b1,c1);

	sympt head = hash_head[14458];
	sympt p = head;
	while(p!=NULL)
	{
		printf("%s\n", p->name);
		p=p->next;
	}


}
