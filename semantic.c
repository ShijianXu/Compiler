#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>I
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

void VarDec(tree *root, Type type, struct Param* para)
{
	
}

void ParamDec(tree* root, fdefpt fun)
{
	//每个ParamDec定义一个形参
	assert(strcmp(root->name, "ParamDec")==0);
	tree* child = root->first_child;
	
	struct Param* para = (struct Param*)malloc(sizeof(struct Param));
	Type type = Specifier(child);	//形参基本类型，但有可能是 数组
	//para->type = type;
	//para->next_para = NULL;
	child = child->next_sibling;
	//ParamDec 第二个参数 VarDec, 获得形参的名和具体类型(basic or array)
	VarDec(child, ,type, para);


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
	//VarList : ParamDec COMMA | ParamDec
	assert(strcmp(root->name, "VarList")==0);
	if(root->child_num == 1)
	{
		tree* child = root->first_child; //ParamDec
		ParamDec(child, fun);
		return;
	}
	else if(root->child_num == 3)
	{
	}
}

fdefpt FunDec(tree *root, Type type_)//type_ 函数返回值类型
{
	fdefpt fun = (fdefpt)malloc(sizeof(FuncDefTableNode));
	//some simplification: the reture type of function can only be INT OR FLOAT
	fun->return_type = type_->basic;	//return_type
	//
	tree *child = root->first_child;
	strcpy(fun->name, child->name);	//func id
	
	child = child->next_sibling->next_sibling; //RP or VarList
	if(strcmp(child->name, "RP")==0)
	{
		fun->para_num = 0;
		fun->para_list = NULL;
		return fun;
	}
	else if(strcmp(child->name,"VarList")==0)
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

void dfs(tree* root)
{
	if(strcmp(root->name, "ExtDef")==0)
	{
		tree *child;
		child = root->first_child;	//Specifier
		Type type = Specifier(child);
		child = child->next_sibling;	//ExtDefList/SEMI/FunDec
		if(strcmp(child->name, "FunDec")==0)
		{
			if(strcmp(child->next_sibling->name,"Compst")==0)	//Fun Definition
			{
				fdefpt = FunDec(child, type);
			}
			else
			{
				//fun declaration
			}
		}
	}
	else
	{
		if(root->first_child != NULL)
			dfs(root->first_child);
		if(root->next_sibling != NULL)
			dfs(root->next_sibling);
	}
}

void semantic_check(tree *root)
{
	init_hash_head();
	dfs(root);
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
	{
		symHashHead[i]=NULL;
		funcDefHashHead[i]=NULL;
		funcDecHashHead[i]=NULL;
		structDefHashHead[i]=NULL;
	}
}

int insert_symtable(sympt node)
{
	unsigned index = hash(node->name);
	printf("%d\n", index);

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
