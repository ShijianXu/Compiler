#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "semantic.h"

int scope = 0;


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
	if(root->child_num == 1)
	{
		if(root->node_kind == GLO_VAR || root->node_kind == FUN_BODY)
		{
			if(root->firstCallVarDec == 1)
			{
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				tree* child = root->first_child;
				strcpy(sym->name, child->value);
				if(root->type->kind == STRUCTURE)
					strcpy(sym->struct_name, root->struct_name);

				sym->type = root->type;
				sym->lineno = child->line;
				unsigned val = insert_symtable(sym);	
				printf("insert return %d\n", val);
			}
			else
			{
				//数组递归调用
				tree* child = root->first_child;
				strcpy(root->syms->name, child->value);
				root->syms->lineno = child->line;
				unsigned val = insert_symtable(root->syms);
				printf("insert_return %d\n", val);
			}
		}
		else if(root->node_kind == FUN_DEC)
		{
		}
		else if(root->node_kind == STR_DEF)
		{
		}
	}
	else
	{
		//root->child_num == 4
		if(root->node_kind == GLO_VAR || root->node_kind == FUN_BODY)
		{
			if(root->firstCallVarDec == 1)
			{
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				sym->type = root->type;
				if(root->type->kind == STRUCTURE)
					strcpy(sym->struct_name, root->struct_name);
				
				tree* child = root->first_child;

				Type type = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = sym->type;
				sym->type = type;
				child->syms = sym;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
			}
			else
			{
				tree* child = root->first_child;
				Type type = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = root->syms->type;
				root->syms->type = type;

				child->syms = root->syms;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
			}
		}
		else if(root->node_kind == FUN_DEC)
		{
		}
		else if(root->node_kind == STR_DEF)
		{
		}
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

void Dec(Type type, tree* root)
{
	//Dec() should insert the var into symbol table
	if(root->child_num == 1)
	{
		//no assign
		tree* child = root->first_child;
		child->type = type;
		VarDec(child);
		//TODO
	}
	else
	{
		//Dec-->VarDec ASSIGNOP Exp
		//TODO
	}
}
/*
void DecList(Type type, tree* root)
{
	if(root->child_num == 1)
	{
		//one var
		tree* child = root->first_child;//Dec
		//TODO
		if(root->structdef == 1)
			child->structdef = 1;
		Dec(type, child);		
	}
	else
	{
		//TODO
		//more than one var
		tree* child = root->first_child;//Dec
		Dec(type, child);
		
		child = child->next_sibling->next_sibling; //DecList
		DecList(type, child);
	}
}
*/
/*
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
		
		DecList(type, child);

		root->stpt = child->stpt;
	}
	else
	{
		//this is not in structure
		//then all the vardec should be inserted into symbol table
		Type type = (Type)malloc(sizeof(struct Type_));	//the type of var
		tree* child = root->first_child; //Specifier
		type = Specifier(child);
		
		child = child->next_sibling;	//DecList
		
		DecList(type, child);
	}
}
*/

/*
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
		child = child->next_sibling;	//DefList
		DefList(child);
	}
}
*/

void StructSpecifier(tree* root)
{
	assert(strcmp(root->name, "StructSpecifier")==0);
	if(root->child_num == 2)
	{
		//structure declaration
		tree* child = root->first_child->next_sibling->first_child;
		printf("struct name is %s\n", child->value);
		strcpy(root->struct_name, child->value);
		return;
	}
	else
	{
		/*
		assert(root->child_num == 5);
		//NOT FINISHED
		//TODO
		spt stpt = (spt)malloc(sizeof(struct StructTableNode));
		//stpt->next will be set when inserted into struct table
		stpt->type = type;

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
		*/
	}
}

void CompSt(tree* root)
{
	//all the var in CompSt should be inserted into symbol table
	tree* child = root->first_child->next_sibling; //DefList
//	DefList(child);
	//TODO
	//child = child->next_sibling; //StmtList
}

void ExtDecList(tree* root)
{
	tree* child = root->first_child;
	
	child->node_kind = root->node_kind;
	child->scope = root->scope;
	child->type = root->type;
	strcpy(child->struct_name, root->struct_name);
	
	child->firstCallVarDec=1;
	VarDec(child);

	if(root->child_num == 1)
		return;
	else
	{
		child = child->next_sibling->next_sibling;
		child->node_kind = root->node_kind;
		child->scope = root->scope;
		child->type = root->type;
		strcpy(child->struct_name, root->struct_name);

		ExtDecList(child);
		return;
	}
}

void dfs(tree* root, int space)
{
	if(strcmp(root->name, "ExtDef")==0)
	{
		//每个ExtDef意味着一个结构体、函数或全局变量的定义
		//C--程序只由这三部分组成
		tree* child;
		child = root->first_child->next_sibling;
		//child == ExtDecList/SEMI/FunDec
		if(strcmp(child->name, "ExtDecList")==0)
		{
			root->node_kind = GLO_VAR;
			root->scope = scope;
			scope+=1;

			child = root->first_child; //Specifier
			child->node_kind = root->node_kind;
			child->scope = root->scope;
			Type type = Specifier(child);
		
			//type如果是int,float，标示全局变量类型为int,float
			//type如果是struct，表示全局变量是个结构体类型，
			//需要进一步访问StructSpecifier来确定结构体类型名
			//这时的产生式为StructSpecifier-->STRUCT Tag
			if(type->kind == STRUCTURE)
			{
				//StructSpecifier()
				tree* grandchild = child->first_child; //StructSpecifier
				grandchild->node_kind = root->node_kind;
				grandchild->scope = root->scope;

				StructSpecifier(grandchild);
				strcpy(root->struct_name, grandchild->struct_name);
			}
			root->type = type;

			child = child->next_sibling; //ExtDecList
			child->node_kind = root->node_kind;
			child->scope = root->scope;
			strcpy(child->struct_name, root->struct_name);

			child->type = root->type;

			//ExtDecList中调用VarDec(),每次判断node_kind，插入符号表
			//ExtDecList(child)
			ExtDecList(child);
		}
		else if(strcmp(child->name, "SEMI")==0)
		{
			root->node_kind = STR_DEF;
			root->scope = scope;
			scope+=1;
			
			child = root->first_child; //Specifier
			child->node_kind = root->node_kind;
			child->scope = root->scope;
			//Type type = Specifier(child);
			//因为知道是结构体定义，所以无需访问Specifier()
			//要继续访问StructSpecifier()获取结构体具体内容
			//这时的产生式为StructSpecifier-->STRUCT OptTag LC DefList RC
			//TODO
			//将结构体定义插入结构体表
		}
		else if(strcmp(child->name, "FunDec")==0)
		{
			root->node_kind = FUN_DEC;
			root->scope = scope;
			scope+=1;

			child = root->first_child; //Specifier
			child->node_kind = root->node_kind;
			child->scope = root->scope;
			Type type = Specifier(child);
			
			//type标示了函数返回值
			//应该只是int或float
			//需要放入函数的return_type中去
			//TODO
			root->type = type;
			
			child = child->next_sibling; //FunDec
			child->node_kind = root->node_kind;
			child->scope = root->scope;
			
			child->type = root->type;

			//FunDec返回之后，将child里记录的函数定义插入函数表和符号表
			//FunDec(child)
			
			child = child->next_sibling; //CompSt
			//CompSt只会出现在函数体中
			child->node_kind = FUN_BODY;
			child->scope = root->scope;
			//root->type就不需要了，内部的变量有自己的type
			
			//CompSt(child)
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
	//check_functable();
	check_symtable();
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

	sympt pt = symHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(node->name, pt->name)==0)
		{
			printf("Error type 3 at Line %d: Redefined variable \"%s\"\n",node->lineno, node->name);
			return -1;
		}
		pt=pt->next;
	}

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

void check_symtable()
{
	for(int i=0; i<hash_size; i++)
	{
		if(symHashHead[i]!=NULL)
		{
			sympt pt = symHashHead[i];
			while(pt!=NULL)
			{
				printf("sym_name:%s  ", pt->name);

				Type type = pt->type;
				while(type->kind != BASIC && type->kind !=STRUCTURE)
				{
					printf("size:%d  ", type->array.size);
					type = type->array.elem;
				}
				if(type->kind == BASIC)
					printf("type:%d  ", type->basic);
				else
					printf("type:%d  ", type->kind);
				if(type->kind == STRUCTURE)
					printf("struct name is %s\n", pt->struct_name);
				else
					printf("\n");
				pt=pt->next;
			}
		}
	}
}
