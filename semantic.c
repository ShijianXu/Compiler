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
		type->kind = STRUCTURE;
		return type;
	}
	else
	{
		printf("Specifier has no other gen\n");
		return NULL;
	}
}

void VarDec(tree *root)
{
	if(root->para->type == NULL)
	//this is the first call of VarDec
	{
		Type type = root->type;	//pre_type records what kind this array is
		root->para->type = type;
	}

	if(root->child_num == 1)
	{
		tree* child = root->first_child;
		strcpy(root->para->name, child->value);
		return;
	}
	else
	{
		assert(root->child_num == 4);
		tree* child = root->first_child;

		Type type = (Type)malloc(sizeof(struct Type_));
		type->kind = ARRAY;
		type->array.size=atoi(child->next_sibling->next_sibling->value);
		type->array.elem = root->para->type;
		root->para->type = type;
		
		child->type = root->type;
		child->para = root->para;
		VarDec(child);

		root->type = child->type;
		root->para = child->para;
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
	
	child->type = type;
	child->para = para;
	VarDec(child);

	struct Param* para2 = child->para;

	fun->para_num += 1;
	if(fun->para_list == NULL)
	{
		fun->para_list = para2;
		para2->next_para = NULL;
	}
	else
	{
		para2->next_para = fun->para_list;
		fun->para_list = para2;
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

void DecList(Type type, tree* root)
{
	if(root->child_num == 1)
	{
		//one var
		//TODO
	}
	else
	{
		//more than one var
		//TODO
	}
}

void Def(tree* root)
{
	//every Def means a definition, like: int a; or: float a,b;
	//if root->structdef==1, vars needs to be added into the stucture
	if(root->structdef == 1)
	{
		//root->stpt is the structure
		tree* child = root->first_child;
		child->structdef = root->structdef;
		child->stpt = root->stpt;

		Type type = (Type)malloc(sizeof(struct Type_));
		type = Specifier(child);
//TODO	

	}
	else
	{
		//this is not in structure
		//then all the vardec should be inserted into symbol table
		Type type = (Type)malloc(sizeof(struct Type_));	//the type of var
		child = root->first_child; //Specifier
		type = Specifier(child);
		
		child = child->next_sibling;	//DecList
		
		DecList(type, child);
	}
}

void DefList(tree* root)
{
	if(root->child_num == 0)	//empty
		return;
	assert(root->child_num == 2);	//Def DefList
	
	if(root->structdef == 1)
	{
		//root->stpt is used;
		tree* child = root->first_child;
		Def(root);

		child = child->next_sibling;
		child->structdef =1;
		child->stpt = root->stpt;

		DefList(child);

		root->stpt = child->stpt;
	}
	else
	{
		//the definition of vars in function
		tree* child = root->first_child;	//Def
		Def(child);
		tree* child = child->next_sibling;	//DefList
		DefList(child);
	}
}

spt StructSpecifier(Type type, tree* root)
{
	assert(strcmp(root->name, "StructSpecifier")==0);
	spt stpt = (spt)malloc(sizeof(struct StructTableNode));
	//stpt->next will be set when inserted into struct table
	stpt->type = type;
	if(root->child_num == 2)
	{
		//structure declaration
		stpt->kind = Declaration;
		tree* child = root->first_child->next_sibling->first_child;
		printf("struct name is %s\n", child->value);
		strcpy(stpt->name, child->value);
		return stpt;
	}
	else
	{
		assert(root->child_num == 5);
		//structure definition
		stpt->kind = Definition;

		tree* child = root->first_child->next_sibling;//OptTag
		
		if(child->child_num !=0)
		{
			assert(child->child_num == 1);	//OptTag
			strcpy(stpt->name, child->first_child->value);
		}
		child = child->next_sibling->next_sibling;//DefList
		child->stpt = stpt;
		child->structdef = 1;
		DefList(child);
		return child->stpt;
	}
}

void CompSt(tree* root)
{
	//all the var in CompSt should be inserted into symbol table
	tree* child = root->first_child->next_sibling; //DefList
	DefList(child);
	//TODO
	//child = child->next_sibling; //StmtList
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
			
			fdefpt func = FunDec(child, type);
			//insert into function table
			int ret_val = insert_funcDefTable(func);
			printf("%d\n", ret_val);
			
			//the statements in the function
			child = child->next_sibling; //CompSt
			CompSt(child);
		}
		else if(strcmp(child->next_sibling->name, "SEMI")==0)
		{
			//It may occur to be 'int;' and this is not wrong.
			//But we do not consider it
			//Just assume this is only the structure definition
			Type type = Specifier(child);
			spt structpt = StructSpecifier(type, child->first_child);
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
