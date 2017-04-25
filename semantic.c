#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "semantic.h"

Type Specifier(tree* root)
{
	Type type = (Type)malloc(sizeof(struct Type_));
	if(strcmp(root->first_child->name,"TYPE")==0)
	{
		if(strcmp(root->first_child->value, "int")==0)
		{
			type->kind = BASIC;
			type->basic = INT_;
			return type;
		}
		else if(strcmp(root->first_child->value, "float")==0)
		{
			type->kind = BASIC;
			type->basic = FLOAT_;
			return type;
		}
		else
		{
			printf("%s\n", root->first_child->value);
			printf("TYPE has no other type.\n");
			return NULL;
		}
	}
	else if(strcmp(root->first_child->name, "StructSpecifier")==0)
	{
		;
	}
	else
	{
		printf("Specifier has no other gen\n");
		return NULL;
	}
}

void VarDec(tree *root, Type pre_type, struct Param* para)
{
	if(para->type == NULL)
	//this is the first call of VarDec
	{
		Type type = pre_type;	//pre_type records what kind this array is
		para->type = type;
	}

	if(root->child_num == 1)
	{
		tree* child = root->first_child;
		strcpy(para->name, child->value);
		return;
	}
	else
	{
		assert(root->child_num == 4);
		tree* child = root->first_child;

		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = ARRAY;
		type->array.size=atoi(child->next_sibling->next_sibling->value);
		type->array.elem = para->type;
		para->type = type;

		VarDec(child, pre_type, para);
		return;
	}
}

void ParamDec(tree* root, fdefpt fun)
{
	//每个ParamDec定义一个形参
	assert(strcmp(root->name, "ParamDec")==0);
	//malloc a param object
	struct Param* para = (struct Param*)malloc(sizeof(struct Param));
	para->type = NULL;

	tree* child = root->first_child; //Specifier
	Type type = Specifier(child);	//形参基本类型，但有可能是 数组

	child = child->next_sibling;	//VarDec, include name, type(int or int[10])
	VarDec(child, type, para);

	fun->para_num += 1;
	if(fun->para_list == NULL)
	{
		fun->para_list = para;
		para->next_para = NULL;
	}
	else
	{
		para->next_para = fun->para_list;
		fun->para_list = para;
	}
}

void VarList(tree* root, fdefpt fun)
{
	//VarList : ParamDec COMMA VarList | ParamDec
	assert(strcmp(root->name, "VarList")==0);
	if(root->child_num == 1)
	{
		tree* child = root->first_child; //ParamDec
		ParamDec(child, fun);
		return;
	}
	else if(root->child_num == 3)
	{
		tree* child = root->first_child;
		ParamDec(child, fun);

		child = child->next_sibling->next_sibling;//Another VarList
		VarList(child, fun);
		return;
	}
}

fdefpt FunDec(tree *root, Type type_)//type_ 函数返回值类型
{
	//malloc an object
	fdefpt fun = (fdefpt)malloc(sizeof(struct FuncDefTableNode));
	//some simplification: the reture type of function can only be INT OR FLOAT
	fun->return_type = type_->basic;//return_type
	fun->para_num = 0;
	//extract the name, paramlist of the function
	tree *child = root->first_child;
	strcpy(fun->name, child->value);	//func id
	
	child = child->next_sibling->next_sibling; //RP or VarList
	if(strcmp(child->name, "RP")==0)	//no parameter
	{
		fun->para_num = 0;
		fun->para_list = NULL;
		return fun;
	}
	else if(strcmp(child->name,"VarList")==0)//param list
	{
		VarList(child, fun);
		return fun;
	}
	else
	{
		printf("FunDec has no other gen.\n");
		return NULL;
	}
}

void dfs(tree* root, int space)
{
/*	if(root->empty == 0)
	{
		for(int i=0; i<space; i++)
			printf(" ");
		printf("%s\n", root->name);
	}
*/

	if(strcmp(root->name, "ExtDef")==0)
	{
		tree *child;
		child = root->first_child;	//Specifier
		
		if(strcmp(child->next_sibling->name, "FunDec")==0)
		{
			Type type = Specifier(child);
			child = child->next_sibling;	//ExtDefList/SEMI/FunDec
			if(strcmp(child->next_sibling->name,"CompSt")==0)	//Fun Definition
			{
				fdefpt func = FunDec(child, type);
				//insert into function table
				int ret_val = insert_funcDefTable(func);
				printf("%d\n", ret_val);
			}
			else
			{
				//fun declaration
			}
		}
		else if(strcmp(child->next_sibling->name, "SEMI")==0)
		{
			//it may occur to be int; but this is not wrong.
			//we do not consider it
			//just assume this is the structure definition
			
			//insert structure
		}
		else if(strcmp(child->next_sibling->name, "ExtDecList")==0)
		{
		}
	}
	
	if(root->first_child != NULL)
		dfs(root->first_child, space+2);
	if(root->next_sibling != NULL)
		dfs(root->next_sibling, space);
}

void semantic_check(tree *root)
{
	init_hash_head();
	printf("checking\n");
	dfs(root, 0);
	check_functable();
	printf("\n\n");
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
	{
		symHashHead[i]=NULL;
		funcDefHashHead[i]=NULL;
		funcDecHashHead[i]=NULL;
		structDefHashHead[i]=NULL;
	}
}

int insert_funcDefTable(fdefpt node)
{
	printf("This fucntion name is %s\n", node->name);
	unsigned index = hash(node->name);
	printf("%d\n", index);
	if(funcDefHashHead[index]==NULL)
	{
		funcDefHashHead[index]=node;
		node->next = NULL;
		return 0;
	}
	else
	{
		node->next = funcDefHashHead[index];
		funcDefHashHead[index]=node;
		return 1;
	}
}

//check whether function has successfully inserted into the table
void check_functable()
{
	for(int i = 0; i < hash_size; i++)
	{
		if(funcDefHashHead[i]!=NULL)
		{
			fdefpt p = funcDefHashHead[i];
			while(p!=NULL)
			{
				printf("function name: %s\n", p->name);
				//specifier 只有基本类型和结构体，然函数不会返回结构体
				printf("return type: %d\n", p->return_type);
				printf("param num: %d\n", p->para_num);
				if(p->para_num != 0)
				{
					printf("params are:\n");
					struct Param* pp = p->para_list;
					while(pp!=NULL)
					{
						printf("	para_name: %s;", pp->name);
						
						Type tp = pp->type;
						printf("param type: %d\n", tp->kind);
						while(tp->kind != BASIC)
						{
							printf("		array_size: %d\n", tp->array.size);
							tp=tp->array.elem;
						}
						if(tp->kind == BASIC)
						{
							printf("		the para's basic type is %d\n", tp->kind);
						}
						pp = pp->next_para;
					}
				}
				p=p->next;
			}
		}
	}
}

int insert_symtable(sympt node)
{
	unsigned index = hash(node->name);
	//printf("%d\n", index);

	if(symHashHead[index] == NULL)
	{
		symHashHead[index] = node;
		node->next = NULL;
		return 0;
	}
	else
	{
		node->next = symHashHead[index];
		symHashHead[index] = node;
		return 1;
	}
}
