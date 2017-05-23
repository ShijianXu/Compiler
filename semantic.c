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
	//			printf("insert return %d\n", val);
			}
			else
			{
				//数组递归调用
				tree* child = root->first_child;
				strcpy(root->syms->name, child->value);
				root->syms->lineno = child->line;
				unsigned val = insert_symtable(root->syms);
			//	printf("insert_return %d\n", val);
			}
		}
		else if(root->node_kind == FUN_DEC)
		{
			if(root->firstCallVarDec == 1)
			{
				struct Param* para = (struct Param*)malloc(sizeof(struct Param));
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				tree* child = root->first_child;
				strcpy(para->name, child->value);
				strcpy(sym->name, child->value);

				if(root->type->kind == STRUCTURE)
					strcpy(sym->struct_name, root->struct_name);

				sym->type = root->type;
				sym->lineno = child->line;
				
				if(root->fun_definition == 1)
					insert_symtable(sym);

				if(root->type->kind == STRUCTURE)
					strcpy(para->struct_name, root->struct_name);
				para->type = root->type;
				//para->lineno = child->line;
				root->para = para;
				return;
			}
			else
			{
				tree* child = root->first_child;
				strcpy(root->para->name, child->value);
				strcpy(root->syms->name, child->value);
				root->syms->lineno = child->line;
				unsigned val = insert_symtable(root->syms);
				//root->para->lineno = child->line;
				return;
			}
		}
		else if(root->node_kind == STR_DEF)
		{
			if(root->firstCallVarDec == 1)
			{
				FieldList field=(FieldList)malloc(sizeof(struct FieldList_));
				tree* child = root->first_child;
				strcpy(field->name, child->value);
				if(root->type->kind == STRUCTURE)
				{
					strcpy(field->struct_name, root->struct_name);
				}
				field->type = root->type;
				
				root->field = field;
			}
			else
			{
				tree* child = root->first_child;
				strcpy(root->field->name, child->value);
			}
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
			if(root->firstCallVarDec == 1)
			{
				struct Param* para = (struct Param*)malloc(sizeof(struct Param));
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				para->type = root->type;
				sym->type = root->type;
				if(root->type->kind == STRUCTURE)
				{
					strcpy(para->struct_name, root->struct_name);
					strcpy(sym->struct_name, root->struct_name);
				}
				tree* child = root->first_child;
				Type type = (Type)malloc(sizeof(struct Type_));
				Type typeS = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				typeS->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				typeS->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = para->type;
				typeS->array.elem = sym->type;
				para->type = type;
				sym->type = typeS;
				child->para = para;
				child->syms = sym;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
				root->para = child->para;
			}
			else
			{
				tree* child = root->first_child;
				Type type = (Type)malloc(sizeof(struct Type_));
				Type typeS = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				typeS->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				typeS->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = root->para->type;
				typeS->array.elem = root->syms->type;
				root->para->type = type;
				root->syms->type = typeS;
				child->para = root->para;
				child->syms = root->syms;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
				root->para = child->para;
			}
		}
		else if(root->node_kind == STR_DEF)
		{
			if(root->firstCallVarDec == 1)
			{
				FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
				field->type = root->type;
				if(root->type->kind == STRUCTURE)
					strcpy(field->struct_name, root->struct_name);
				tree* child = root->first_child;
				Type type = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = field->type;
				field->type = type;
				child->field = field;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
				root->field = child->field;
			}
			else
			{
				tree* child = root->first_child;
				Type type = (Type)malloc(sizeof(struct Type_));
				type->kind = ARRAY;
				type->array.size = atoi(child->next_sibling->next_sibling->value);
				type->array.elem = root->field->type;
				root->field->type = type;
				child->field = root->field;
				child->firstCallVarDec = 0;
				child->node_kind = root->node_kind;
				VarDec(child);
				root->field = child->field;
			}
		}
	}
}

void ParamDec(tree* root)
{
	//每个ParamDec定义一个形参
	assert(strcmp(root->name, "ParamDec")==0);

	tree* child = root->first_child; //Specifier
	Type type = Specifier(child);	//basic, strcut, 但有可能是数组
	if(type->kind == STRUCTURE)
	{
		//StructSpecifier()
		tree* grandchild = child->first_child; //StructSpecifier
		grandchild->node_kind = root->node_kind;
		grandchild->scope = root->scope;

		StructSpecifier(grandchild);
		strcpy(root->struct_name, grandchild->struct_name);
	}
	child = child->next_sibling;	//VarDec, include name, type(int or int[10])
	child->type = type;
	child->fun_definition = root->fun_definition;
	child->node_kind = root->node_kind;
	child->firstCallVarDec = 1;
	strcpy(child->struct_name, root->struct_name);
	VarDec(child);
	struct Param* para2 = child->para;

	root->func->para_num += 1;
	if(root->func->para_list == NULL)
	{
		root->func->para_list = para2;
		para2->next_para = NULL;
	}
	else
	{
		para2->next_para = root->func->para_list;
		root->func->para_list = para2;
	}
}

void VarList(tree* root)
{
	//VarList : ParamDec COMMA VarList | ParamDec
	assert(strcmp(root->name, "VarList")==0);
	
	tree* child = root->first_child;
	child->func = root->func;
	child->fun_definition = root->fun_definition;
	child->node_kind = root->node_kind;
	ParamDec(child);
	root->func = child->func;

	if(root->child_num == 1)
		return;
	else
	{
		//root->child_num == 3
		child = child->next_sibling->next_sibling;//Another VarList
		child->func = root->func;
		child->fun_definition = root->fun_definition;
		child->node_kind = root->node_kind;
		VarList(child);
		return;
	}
}

void insertReadWrite()
{
	fdefpt read=(fdefpt)malloc(sizeof(struct FuncDefTableNode));
	fdefpt write=(fdefpt)malloc(sizeof(struct FuncDefTableNode));

	strcpy(read->name, "read");
	strcpy(write->name, "write");

	read->return_type = INT_;
	write->return_type = INT_;

	read->para_num = 0;
	read->para_list = NULL;

	write->para_num = 1;

	struct Param* para=(struct Param*)malloc(sizeof(struct Param));
	Type type=(Type)malloc(sizeof(struct Type_));
	type->kind = BASIC;
	type->basic = INT_;
	para->type = type;
	para->next_para = NULL;
	strcpy(para->name, "n");
	write->para_list = para;

	read->next = NULL;
	write->next = NULL;

	unsigned val1 = insert_funcDefTable(read);
	unsigned val2 = insert_funcDefTable(write);

//	printf("insert read return %d\ninsert write return %d\n", val1, val2);
	sympt sym1=(sympt)malloc(sizeof(struct SymTableNode));
	sympt sym2=(sympt)malloc(sizeof(struct SymTableNode));

	strcpy(sym1->name,"read");
	strcpy(sym2->name,"write");
	
	Type type1=(Type)malloc(sizeof(struct Type_));
	Type type2=(Type)malloc(sizeof(struct Type_));
	type1->kind = FUNCTION;
	type2->kind = FUNCTION;
	sym1->type = type1;
	sym2->type = type2;

	val1 = insert_symtable(sym1);
	val2 = insert_symtable(sym2);
//	printf("insert readsym return %d\ninsert writesym return%d\n", val1, val2);
}

void FunDec(tree *root)//root->type记录函数返回值
{
	fdefpt fun = (fdefpt)malloc(sizeof(struct FuncDefTableNode));
	//some simplification: the reture type of function can only be INT OR FLOAT
	fun->return_type = root->type->basic;//return_type, int/float
	fun->para_num = 0;
	
	tree *child = root->first_child;
	strcpy(fun->name, child->value);	//func id
	
	child = child->next_sibling->next_sibling; //RP or VarList
	if(strcmp(child->name, "RP")==0)	//no parameter
	{
		fun->para_num = 0;
		fun->para_list = NULL;
		fun->lineno = root->line;
	
		if(root->fun_definition == 1)
		{
			unsigned val = insert_funcDefTable(fun);
		
			if(val!=-1)
			{
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				strcpy(sym->name, fun->name);
				Type sym_type = (Type)malloc(sizeof(struct Type_));
				sym_type->kind = FUNCTION;
				sym->type = sym_type;
				insert_symtable(sym);
			}
		}
		else
		{
			unsigned val = insert_funcDecTable(fun);
			if(val == -1)
			{
				printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", child->line, fun->name);
			}
		}
//		printf("insert fun return %d\n", val);
	}
	else if(strcmp(child->name,"VarList")==0)//param list
	{
		fun->para_num = 0;
		fun->para_list = NULL;
		fun->lineno = child->line;

		child->node_kind = root->node_kind;
		child->func = fun;
		child->fun_definition = root->fun_definition;
		VarList(child);
	
		if(root->fun_definition == 1)
		{
			unsigned val = insert_funcDefTable(child->func);
			if(val != -1)
			{
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				strcpy(sym->name, fun->name);
				Type sym_type = (Type)malloc(sizeof(struct Type_));
				sym_type->kind = FUNCTION;
				sym->type = sym_type;
				insert_symtable(sym);
			}
		}
		else
		{
			unsigned val = insert_funcDecTable(child->func);
			if(val == -1)
			{
				printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", child->line, fun->name);
			}
		}

	//	sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
	//	strcpy(sym->name, child->func->name);
	//	sym->type = child->func->return_type;

//		printf("insert fun return %d\n", val);
	}
}

void Dec(tree* root)
{
	if(root->node_kind == STR_DEF)
	{
		tree* child = root->first_child;
		child->node_kind = root->node_kind;
		child->type = root->type;
		child->stpt = root->stpt;
		strcpy(child->struct_name, root->struct_name);	
		child->firstCallVarDec = 1;
		//每个VarDec NEW一个field对象
		VarDec(child);
	
		FieldList field = child->stpt->fieldList;
		int redef = 0;
		while(field!=NULL)
		{
			if(strcmp(child->field->name, field->name)==0)
			{
				printf("Error type 15 at Line %d: Redefined field \"%s\".\n", child->line, child->field->name);
				redef = 1;
				break;
			}
			field = field->next;
		}
		if(redef == 0)
		{
			if(child->stpt->fieldList == NULL)
			{
				child->stpt->fieldList = child->field;
				child->field->next = NULL;
			}
			else
			{
				child->field->next = child->stpt->fieldList;
				child->stpt->fieldList = child->field;
			}
		}
		if(root->child_num == 3)
		{
			printf("Error type 15 at Line %d: struct field should NOT be initialized.\n", child->line);
		}
		root->stpt = child->stpt;
		return;
	}
	else if(root->node_kind == FUN_BODY)
	{
		tree* child = root->first_child;
		child->node_kind = root->node_kind;
		child->type = root->type;
		strcpy(child->struct_name, root->struct_name);
		child->firstCallVarDec = 1;
		VarDec(child);

		if(root->child_num == 1)
			return;
		else
		{
			//root->child_num == 3
			//VarDec ASSIGNOP Exp
			child = root->first_child->first_child;//ID
			sympt sym = lookup_sym(child);

			child = root->first_child->next_sibling->next_sibling; //Exp
			child->node_kind = root->node_kind;
			Exp(child);

			switch(child->exp_type)
			{
				case _INT:
				{
					if(sym->type->kind != BASIC || (sym->type->kind == BASIC && sym->type->basic != INT_))
						printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
					break;
				}
				case _FLOAT:
				{
					if(sym->type->kind != BASIC || (sym->type->kind == BASIC && sym->type->basic != FLOAT_))
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
					break;
				}
				default:
				{
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
				}
			}
		}
	}
}

void DecList(tree* root)
{
	tree* child = root->first_child;//Dec
	child->node_kind = root->node_kind;
	child->stpt = root->stpt;
	child->scope = root->scope;
	strcpy(child->struct_name, root->struct_name);
	child->type = root->type;

	Dec(child);
	
	if(root->child_num == 1)
	{
		if(root->node_kind == STR_DEF)
		{
			root->stpt = child->stpt;
			return;
		}
		else if(root->node_kind == FUN_BODY)
		{
			return;
		}
	}
	else
	{
		//root->child_num ==3
		child = child->next_sibling->next_sibling; //DecList
		child->node_kind = root->node_kind;
		child->scope = root->scope;
		child->type = root->type;
		child->stpt = root->stpt;
		strcpy(child->struct_name, root->struct_name);
		DecList(child);
		
		if(root->node_kind == STR_DEF)
		{
			root->stpt = child->stpt;
			return;
		}
		else if(root->node_kind == FUN_BODY)
		{
			return;
		}
	}
}

void Def(tree* root)
{
	//每个DEF对应一组变量的定义: int a,b,c;
	//只有在后续调用到Dec之后，才是一个变量的定义
	if(root->node_kind == STR_DEF)
	{
		tree* child = root->first_child;	//specifier
		Type type = (Type)malloc(sizeof(struct Type_));
		type = Specifier(child);
	
		if(type->kind == STRUCTURE)
		{
			tree* child0 = root->first_child->first_child;
			
			if(child0->child_num == 2)
			{
			tree* grandchild=child->first_child; //StructSpecifier
			grandchild->node_kind = root->node_kind;
			grandchild->scope = root->scope;

			StructSpecifier(grandchild);
			strcpy(root->struct_name, grandchild->struct_name);
			}
			else
			{
				printf("child_num:%d\n", child0->child_num);
				//STRUCT OptTag LC DefList RC
				spt stpt = (spt)malloc(sizeof(struct StructTableNode));
				child0->node_kind = STR_DEF;//STR_DEF
				child0->scope = root->scope;
				child0->stpt = stpt;
				StructSpecifier(child0);
				
				child0->stpt->lineno = child0->line;
				

				//unsigned val = insert_structTable(child0->stpt);
				type->structDefpt = child0->stpt;
				//TODO TODO HAVE DONE, JUST FOR ATTENTION
				//嵌套定义
				
			/*	FieldList fl = type->structDefpt->fieldList;
				while(fl!=NULL)
				{
					printf("fl name: %s\n", fl->name);
					fl = fl->next;
				}*/

			}
		}
		root->type = type;
		
		child = child->next_sibling; //DecList
		child->node_kind = root->node_kind;
		child->stpt = root->stpt;
		strcpy(child->struct_name, root->struct_name);
		child->type = root->type;

		DecList(child);

		root->stpt = child->stpt;
	}
	else if(root->node_kind == FUN_BODY)
	{
		//this is not in structure
		//then all the vardec should be inserted into symbol table
		Type type = (Type)malloc(sizeof(struct Type_));	//the type of var
		tree* child = root->first_child; //Specifier
		type = Specifier(child);
		
		if(type->kind == STRUCTURE)
		{
			tree* grandchild=child->first_child; //StructSpecifier
			grandchild->node_kind = root->node_kind;
			grandchild->scope = root->scope;

			StructSpecifier(grandchild);
			strcpy(root->struct_name, grandchild->struct_name);
		}
		root->type = type;
	
		child = child->next_sibling;	//DecList
		child->type = root->type;
		child->node_kind = root->node_kind;
		strcpy(child->struct_name, root->struct_name);
		
		DecList(child);
	}
}

void DefList(tree* root)
{
	if(root->child_num == 0)	//empty
		return;

	if(root->node_kind == STR_DEF)
	{
		//root->stpt is used;
		tree* child = root->first_child;
		child->node_kind = root->node_kind;
		child->stpt = root->stpt;
		child->scope = root->scope;

		Def(child);
		root->stpt = child->stpt;

		child = child->next_sibling;//DefList
		child->node_kind = root->node_kind;
		child->stpt = root->stpt;
		child->scope = root->scope;

		DefList(child);
		root->stpt = child->stpt;
	}
	else if(root->node_kind == FUN_BODY)
	{
		//the definition of vars in function
		tree* child = root->first_child;	//Def
		child->node_kind = root->node_kind;
		child->return_type = root->return_type;
		Def(child);
		child = child->next_sibling;	//DefList
		child->node_kind = root->node_kind;
		child->return_type = root->return_type;
		DefList(child);
	}
}


void StructSpecifier(tree* root)
{
	assert(strcmp(root->name, "StructSpecifier")==0);
	if(root->child_num == 2)
	{
		//structure object declaration
		tree* child = root->first_child->next_sibling->first_child;
	//	printf("struct name is %s\n", child->value);
		strcpy(root->struct_name, child->value);

		spt structpt = lookup_struct(root);
		if(structpt == NULL)
		{
			printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", child->line, child->value);
		}
		return;
	}
	else
	{
		assert(root->child_num == 5);
	
		//structure definition
		tree* child = root->first_child->next_sibling;//OptTag
		
		if(child->child_num !=0)
		{
			assert(child->child_num == 1);	//OptTag
			strcpy(root->stpt->name, child->first_child->value);
		}

		child = child->next_sibling->next_sibling;//DefList
		child->stpt = root->stpt;
		child->node_kind = root->node_kind;//STR_DEF
		child->scope = root->scope;
		DefList(child);
		
		root->stpt = child->stpt;
	}
}

void Exp(tree* root)
{	
	if(root->child_num == 1)
	{
		tree* child = root->first_child;//ID INT FLOAT
		if(strcmp(child->name, "ID")==0)
		{
			sympt sym = lookup_sym(child);

			if(sym!=NULL)
			{
				if(sym->type->kind == BASIC)
				{
					if(sym->type->basic == INT_)
						root->exp_type = _INT;
					else if(sym->type->basic == FLOAT_)
						root->exp_type = _FLOAT;
				}
				else if(sym->type->kind == ARRAY)
				{
					root->exp_type = _ARRAY;
					
					int dim = 0;
					Type type = sym->type;
					while(type->kind != BASIC && type->kind != STRUCTURE)
					{
						dim +=1;	
						type = type->array.elem;
					}
					if(type->kind == BASIC)
					{
						if(type->basic == INT_)
							root->array_basic_type = _INT;
						else if(type->basic == FLOAT_)
							root->array_basic_type = _FLOAT;
					}
					else if(type->kind == STRUCTURE)
					{
						root->array_basic_type = _STRUCTURE;
						strcpy(root->struct_name, sym->struct_name);
					}
					root->array_dim = dim;
				}
				else if(sym->type->kind == STRUCTURE)
				{
					root->exp_type = _STRUCTURE;
					strcpy(root->struct_name, sym->struct_name);
				}
				else if(sym->type->kind == FUNCTION)
				{
					root->exp_type = _FUNC;
				}
				else
				{
					root->exp_type = _NONE;
					printf("other exp_type\n");
				}
			}
			else
			{
				printf("Error type 1 at Line %d: Undefined variable \"%s\"\n", child->line, child->value);
				root->exp_type = _NONE;
			}
		}
		else if(strcmp(child->name,"INT")==0)
		{
			root->exp_type = _INT;
			root->isNum = 1;
		}
		else if(strcmp(child->name,"FLOAT")==0)
		{
			root->exp_type = _FLOAT;
			root->isNum = 1;
		}
	}
	else if(root->child_num == 2)
	{
		//NOT Exp
		//MINUS Exp
		tree *child = root->first_child;
		if(strcmp(child->name, "MINUS")==0)
		{
			child = child->next_sibling;
			Exp(child);
			root->exp_type = child->exp_type;
			root->isNum = child->isNum;
			root->array_dim = child->array_dim;
			root->array_basic_type = child->array_basic_type;
			root->args = child->args;
			root->return_type = child->return_type;
		}
		else if(strcmp(child->name, "NOT")==0)
		{
			child = child->next_sibling;
			Exp(child);
			root->exp_type = child->exp_type;
			root->isNum = child->isNum;
			root->array_dim = child->array_dim;
			root->array_basic_type = child->array_basic_type;
			root->args = child->args;
			root->return_type = child->return_type;
		}
	}
	else if(root->child_num == 3)
	{
		tree* child = root->first_child->next_sibling;
		if(strcmp(child->name,"ASSIGNOP")==0)
		{
			//Exp ASSIGNOP Exp
			child = root->first_child;
			Exp(child);
			
			if(child->isNum)
			{
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", child->line);
			}
			tree* child2 = child->next_sibling->next_sibling;
			assert(strcmp(child2->name, "Exp")==0);
			Exp(child2);

			tree* child0 = root->first_child;

			if(child->exp_type == _ARRAY && child0->child_num != 1)
			{
				if(child->array_basic_type != child2->exp_type)
				{
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
				}
			}
			else if(child->exp_type == _STRUCTURE && child2->exp_type == _STRUCTURE)
			{
				spt structpt1 = lookup_struct(child);
				spt structpt2 = lookup_struct(child2);

				FieldList fl1 = structpt1->fieldList;
				FieldList fl2 = structpt2->fieldList;

				int str_equ = 1;
				while(fl1!=NULL || fl2!=NULL)
				{
					if((fl1==NULL && fl2 != NULL) ||(fl1!=NULL && fl2 ==NULL))
					{
						str_equ = 0;
						break;
					}
					else
					{
						if(fl1->type->kind == fl2->type->kind)
						{
							if(fl1->type->kind == BASIC)
							{
								if(fl1->type->basic != fl2->type->basic)
								{
									str_equ = 0;
									break;
								}
							}
							else if(fl1->type->kind == ARRAY)
							{
								Type t1 = fl1->type;
								Type t2 = fl2->type;
								int dim1 = 0;
								int dim2 = 0;
								int size_eq = 1;
								while(t1->kind!=BASIC && t1->kind !=STRUCTURE)
								{dim1+=1;t1=t1->array.elem;}
								while(t2->kind!=BASIC && t2->kind !=STRUCTURE)
								{dim2+=1;t2=t2->array.elem;}
								if(dim1!=dim2)
								{ str_equ = 0;break; }
								else
								{
									t1 = fl1->type; t2 = fl2->type;
									while(t1->kind!=BASIC && t1->kind != STRUCTURE)
									{
										if(t1->array.size != t2->array.size)
										{
											size_eq=0;
											break;
										}
										t1=t1->array.elem;
										t2=t2->array.elem;
									}
									if(size_eq==0)
									{
										str_equ = 0;break;
									}
									else if(t1->kind!=t2->kind || (t1->kind==BASIC && t2->kind==BASIC &&t1->basic!=t2->basic))
									{
										str_equ = 0;break;
									}
									else if(t1->kind==STRUCTURE && t2->kind==STRUCTURE)
									{
										if(strcmp(fl1->struct_name, fl2->struct_name)!=0) { str_equ = 0;break; }
									}
								}
							}
							else if(fl1->type->kind == STRUCTURE)
							{
								//TODO TOO COMPLECATED
							}
						}
						else
						{
							str_equ = 0;
							break;
						}
					}
					fl1=fl1->next;
					fl2=fl2->next;
				}

				if(str_equ == 0)
				{
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
				}
			/*	else if(strcmp(child->struct_name, child2->struct_name)!=0)
				{
					printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
				}*/
			}
			else if(child->exp_type != _NONE && child->exp_type != child2->exp_type)
				printf("Error type 5 at Line %d: Type mismatched for assignment.\n", child->line);
			root->exp_type = child->exp_type;
		}
		else if(strcmp(child->name, "RELOP")==0)
		{
			child = root->first_child;
			Exp(child);
			
			tree* child2 = child->next_sibling->next_sibling;
			Exp(child2);
			if(child->exp_type != child2->exp_type)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", child->line);
			}
			else
			{
				root->exp_type = child->exp_type;
			}
		}
		else if(strcmp(child->name, "AND") == 0 || strcmp(child->name,"OR")==0)
		{
			child = root->first_child;
			Exp(child);

			tree* child2 = child->next_sibling->next_sibling;
			Exp(child2);


			if(child->exp_type != _INT || child2->exp_type != _INT)
			{
				printf("Error type 7 at Line %d: Type mismatched for \"&&\", only INT can.\n", child->line);
			}
			else
			{
				root->exp_type = _INT;
			}
		}
		else if(strcmp(child->name, "Exp")==0)
		{
			//LP Exp RP
			Exp(child);
			root->exp_type = child->exp_type;
		}
		else if(strcmp(child->name, "PLUS")==0 ||
				strcmp(child->name, "MINUS")==0 ||
				strcmp(child->name, "STAR")==0 ||
				strcmp(child->name, "DIV")==0 )
		{
			tree* child = root->first_child;
			tree* child2 = child->next_sibling->next_sibling;
			Exp(child);
			Exp(child2);

			if(child->array_basic_type!=_NONE)
				child->exp_type = child->array_basic_type;
			if(child2->array_basic_type !=_NONE)
				child2->exp_type = child2->array_basic_type;

			if((child->exp_type != _INT && child->exp_type != _FLOAT) || (child2->exp_type != _INT && child2->exp_type != _FLOAT) || child->exp_type != child2->exp_type)
			{
				printf("Error type 7 at Line %d: Type mismatched for operands.\n", child->line);
			}
			else
			{
				if(child->exp_type == _INT)
					root->exp_type = _INT;
				else if(child->exp_type == _FLOAT)
					root->exp_type = _FLOAT;
			}
		}
		else if(strcmp(child->name,"DOT")==0)
		{
			//Exp DOT ID
			tree* child = root->first_child;//Exp
			Exp(child);
			if(child->exp_type == _ARRAY)
			{
				if(child->array_basic_type != _STRUCTURE)
					printf("Error type 13 at Line %d: Illegal use of \".\"\n", child->line);
			}
			else if(child->exp_type != _STRUCTURE)
			{
				printf("Error type 13 at Line %d: Illegal use of \".\"\n", child->line);
			}
			else
			{
				spt structpt = lookup_struct(child);
				if(structpt == NULL)
				{
					printf("Error struct not found\n");
				}
				else
				{	
					child = child->next_sibling->next_sibling;//ID
					FieldList field = structpt->fieldList;
					while(field!= NULL)
					{
						if(strcmp(field->name, child->value)==0)
						{
							switch(field->type->kind)
							{
								case BASIC:
								{
									if(field->type->basic == INT_)
										root->exp_type = _INT;
									else
										root->exp_type = _FLOAT;
									break;
								}
								case ARRAY:
								{
									root->exp_type = _ARRAY;
									break;
								}
								case STRUCTURE:
								{
									root->exp_type = _STRUCTURE;
									break;
								}
								default:
									printf("Error: other struct field type.\n");
							}
							break;
						}
						field = field->next;
					}
					if(field == NULL)
					{
						printf("Error type 14 at Line %d: Non-existent field \"%s\".\n", child->line,child->value);
					}
				}
			}
		}
		else if(strcmp(child->name,"LP")==0)
		{
			//ID LP RP
			tree* child = root->first_child; //ID
			Exp(child);
			root->exp_type = child->exp_type;
			
			sympt sym = lookup_sym(child);
			fdefpt func = lookup_func(child);
			if(sym==NULL)
			{
				printf("Error type 2 at Line %d: Undefined function \"%s\"\n",child->line, child->value);
			}
			else if(sym->type->kind != FUNCTION)
			{
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", child->line, child->value);
			}
			else
			{
				//函数存在
				switch(func->return_type)
				{
					case INT_:
					{
						root->exp_type = _INT;
						break;
					}
					case FLOAT_:
					{
					root->exp_type = _FLOAT;
						break;
					}
					default:
					{
						printf("Error other func return type.\n");
					}
				}
			}
		}
	}
	else if(root->child_num == 4)
	{
		tree* child = root->first_child;
		if(strcmp(child->name,"ID")==0)
		{
			//ID LP Args RP
			sympt sym = lookup_sym(child);

			fdefpt func = lookup_func(child);
			if(sym==NULL)
			{
				printf("Error type 2 at Line %d: Undefined function \"%s\"\n",child->line, child->value);
			}
			else if(sym->type->kind != FUNCTION)
			{
				printf("Error type 11 at Line %d: \"%s\" is not a function.\n", child->line, child->value);
			}
			else
			{
				child = child->next_sibling->next_sibling;
				assert(strcmp(child->name, "Args")==0);
				
				child->args = NULL;
				int args_num = Args(child,0);

				if(args_num != func->para_num)
				{
					printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, args num not right.\n",child->line, func->name);
				}
				else
				{
					struct ArgsType* at = child->args;
					struct Param* para = func->para_list;
					while(para!=NULL)
					{
						switch(at->et)
						{
							case _INT:
							{
								if(para->type->kind!= BASIC || (para->type->kind == BASIC && para->type->basic != INT_))
								printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, args type not right.\n",child->line, func->name);
								break;
							}
							case _FLOAT:
							{
								if(para->type->kind!= BASIC || (para->type->kind == BASIC && para->type->basic != FLOAT_))
								printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments args type not right.\n",child->line, func->name);
								break;
							}
							case _ARRAY:
							{
								Type type = para->type;
								int dim = 0;
								while(type->kind != BASIC && type->kind != STRUCTURE)
								{
									dim +=1;
									type = type->array.elem;
								}
								if(para->type->kind != ARRAY)
									printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, array.\n",child->line, func->name);
								else if(dim != at->array_dim)
									printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, the dim of array is wrong.\n",child->line, func->name);	
								break;
							}
							case _STRUCTURE:
							{
								if(para->type->kind != STRUCTURE)
									printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, structure.\n",child->line, func->name);
								break;
							}
							case _NONE:
							{
								printf("Error type 9 at Line %d: Function \"%s\" is not applicable for arguments, _NONE.\n",child->line, func->name);
								break;
							}
							default:
							{
								printf("Error: other type args.\n");
							}
						}
						at=at->next;
						para=para->next_para;
					}
				}
				switch(func->return_type)
				{
					case INT_:
					{
						root->exp_type = _INT;
						break;
					}
					case FLOAT_:
					{
					root->exp_type = _FLOAT;
						break;
					}
					default:
					{
						printf("Error other func return type.\n");
					}
				}
			}
		}
		else
		{
			//Exp LB Exp RB
			tree* child = root->first_child;
			Exp(child);
			root->exp_type = child->exp_type;

			root->array_dim = child->array_dim;
			root->array_basic_type = child->array_basic_type;

			tree* child2 = root->first_child->first_child;
			if(strcmp(child2->name,"ID")==0)
			{
				if(child->exp_type != _ARRAY)
				{
					printf("Error type 10 at Line %d: This is not an array.\n", child->line);
				}
				else
				{
					root->exp_type = child->array_basic_type;

					child = child->next_sibling->next_sibling;
					Exp(child);
					if(child->exp_type != _INT)
					{
						printf("Error type 12 at Line %d: This number is not an integer.\n", child->line);
					}
				}
			}
		}
	}
}

int Args(tree* root, int num)
{
	tree* child = root->first_child;//Exp
	Exp(child);
	num += 1;

	struct ArgsType* args = (struct ArgsType*)malloc(sizeof(struct ArgsType));

	//
	
	args->et = child->exp_type;
	//args->et = child->array_basic_type;
	if(args->et == _ARRAY)
	{
		args->array_dim = child->array_dim;
	}

	if(root->args == NULL)
	{
		root->args = args;
		args->next = NULL;
	}
	else
	{
		args->next = root->args;
		root->args = args;
	}

	if(root->child_num == 1)
	{
		return num;
	}
	else
	{
		tree* child2 = child->next_sibling->next_sibling;//Args
		child2->args = root->args;
		num = Args(child2,num);
		root->args = child2->args;

		return num;
	}
}

void Stmt(tree* root)
{
	if(root->child_num == 1)//CompSt
	{
		tree* child = root->first_child;
		child->node_kind = root->node_kind;
		CompSt(child);
	}
	else if(root->child_num == 2) //Exp SEMI
	{
		tree* child = root->first_child;
		child->node_kind = root->node_kind;
		
		Exp(child);
	}
	else if(root->child_num == 3) //RETURN Exp SEMI
	{
		tree* child = root->first_child->next_sibling;//Exp
		child->node_kind = root->node_kind;
		Exp(child);
		
		switch(child->exp_type)
		{
			case _INT:
			{
				if(root->return_type->kind != BASIC || (root->return_type->kind == BASIC && root->return_type->basic != INT_))
					printf("Error type 8 at Line %d: Type mismatched for return, should be int.\n", child->line);
				break;
			}
			case _FLOAT:
			{
				if(root->return_type->kind != BASIC || (root->return_type->kind == BASIC && root->return_type->basic != FLOAT_))
					printf("Error type 8 at Line %d: Type mismatched for return, should be float.\n", child->line);
				break;
			}
			default:
				printf("Error type 8 at Line %d: Type mismatched for return, neither int nor float.\n", child->line);
		}
	}
	else if(root->child_num == 5) 
			//IF LP Exp RP Stmt
			//WHILE LP Exp RP Stmt
	{
		tree* child = root->first_child;
		if(strcmp(child->name,"IF")==0)
		{
			child = child->next_sibling->next_sibling;//Exp
			child->node_kind = root->node_kind;
			Exp(child);

			child = child->next_sibling->next_sibling;
			child->return_type = root->return_type;
			child->node_kind = root->node_kind;
			Stmt(child);
		}
		else
		{
			assert(strcmp(child->name,"WHILE")==0);
			child = child->next_sibling->next_sibling;
			child->node_kind = root->node_kind;
			Exp(child);

			child = child->next_sibling->next_sibling;
			child->return_type = root->return_type;
			child->node_kind = root->node_kind;
			Stmt(child);
		}
	}
	else if(root->child_num == 7)//IF LP Exp RP Stmt ELSE Stmt
	{
		tree* child = root->first_child->next_sibling->next_sibling;
		Exp(child);
		
		child = child->next_sibling->next_sibling;
		child->return_type = root->return_type;
		Stmt(child);

		child = child->next_sibling->next_sibling;
		child->return_type = root->return_type;
		Stmt(child);
	}
}

void StmtList(tree* root)
{
	if(root->child_num == 0)
		return;
	else
	{
		tree* child = root->first_child; //Stmt
		child->node_kind = root->node_kind;
		child->return_type = root->return_type;
		Stmt(child);

		child = child->next_sibling;
		child->node_kind = root->node_kind;
		child->return_type = root->return_type;
		StmtList(child);
	}
}

void CompSt(tree* root)
{
	//all the var in CompSt should be inserted into symbol table
	tree* child = root->first_child->next_sibling; //DefList
	child->node_kind = root->node_kind;
	child->return_type = root->return_type;
	DefList(child);
	
	child = child->next_sibling; //StmtList
	child->node_kind = root->node_kind;
	child->return_type = root->return_type;

	StmtList(child);
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
	if(strcmp(root->name, "CheckPoint") == 0)
	{
		CheckPoint();
	}
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
			
			child=root->first_child->first_child;//StructSpecifier
			child->node_kind = root->node_kind;//STR_DEF
			child->scope = root->scope;
			
			spt stpt = (spt)malloc(sizeof(struct StructTableNode));
			child->stpt = stpt;
			StructSpecifier(child);
		
			child->stpt->lineno = child->line;
			unsigned val = insert_structTable(child->stpt);
			//printf("insert struct return %d\n", val);
		/*	if(val != -1)
			{
				sympt sym = (sympt)malloc(sizeof(struct SymTableNode));
				strcpy(sym->name, child->stpt->name);
				Type sym_type = (Type)malloc(sizeof(struct Type_));
				sym_type->kind = STRUCTURE;
				sym->type = sym_type;
				insert_symtable(sym);
			}*/
		}
		else if(strcmp(child->name, "FunDec")==0)
		{
			root->node_kind = FUN_DEC;
			root->scope = scope;
			scope+=1;

			child = root->first_child; //Specifier
			child->node_kind = FUN_DEC;
			child->scope = root->scope;
			Type type = Specifier(child);
			
			//type标示了函数返回值
			//应该只是int或float
			//需要放入函数的return_type中去
	
			child = child->next_sibling; //FunDec
			child->type = type;	//函数返回值
			child->node_kind = FUN_DEC;
			child->scope = root->scope;
		
			tree* child2 = child->next_sibling;
			if(strcmp(child2->name,"CompSt")==0)
				child->fun_definition = 1;
			else if(strcmp(child2->name, "SEMI")==0)
				child->fun_definition = 0;

			FunDec(child);
			//-----------------------------
			if(child->fun_definition == 1)
			{
			child = child->next_sibling; //CompSt
			child->node_kind = FUN_BODY;
			child->scope = root->scope;

			child->return_type = type;
			CompSt(child);
			}
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
//	printf("checking\n");	
	insertReadWrite();
	dfs(root, 0);
//	check_functable();
//printf("----------check symtable----------\n");
//	check_symtable();
//printf("----------check structtable--------\n");
//	check_structtable();
//	printf("\n\n");
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

int insert_funcDecTable(fdefpt node)
{
	unsigned index = hash(node->name);
	fdefpt pt = funcDecHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(pt->name, node->name)==0)
		{
			if(pt->return_type != node->return_type)
				return -1;

			struct Param* para1 = pt->para_list;
			struct Param* para2 = node->para_list;
			int para_conflict = 0;
			while(para1 != NULL || para2 != NULL)
			{
				if((para1==NULL && para2!=NULL) || (para1!=NULL && para2==NULL))
				{
					para_conflict = 1;
					break;
				}
				if(para1->type->kind != para2->type->kind)
				{
					para_conflict = 1;
					break;
				}
				else if(para1->type->kind == BASIC)
				{
					if(para1->type->basic != para2->type->basic)
					{
						para_conflict = 1;
						break;
					}
				}
				else if(para1->type->kind == STRUCTURE)
				{
					if(strcmp(para1->struct_name, para2->struct_name)!=0)
					{
						para_conflict = 1;
						break;
					}
				}
				else if(para1->type->kind == ARRAY)
				{
					Type type1 = para1->type;
					Type type2 = para2->type;
					int type_error = 0;
					while((type1->kind != BASIC && type1->kind != STRUCTURE) || (type2->kind !=BASIC && type2->kind != STRUCTURE))
					{
						if(((type1->kind==BASIC || type1->kind == STRUCTURE) && (type2->kind!=BASIC && type2->kind!=STRUCTURE)) || ((type2->kind==BASIC || type2->kind == STRUCTURE) && (type1->kind!=BASIC && type1->kind!=STRUCTURE)))
						{
							type_error = 1;
							break;
						}
						type1=type1->array.elem;
						type2=type2->array.elem;
					}
					if(type_error == 1)
					{
						para_conflict = 1;
						break;
					}
					else
					{
						if(type1->kind!=type2->kind)
						{
							para_conflict = 1;
							break;
						}
					}
				}
				para1 = para1->next_para;
				para2 = para2->next_para;
			}
			if(para_conflict == 1)
				return -1;
			else
				return 1;
		}
		pt=pt->next;
	}

	if(funcDecHashHead[index]==NULL)
	{
		funcDecHashHead[index]=node;
		node->next = NULL;
		return 0;
	}
	else
	{
		node->next = funcDecHashHead[index];
		funcDecHashHead[index]=node;
		return 0;
	}
}

int insert_funcDefTable(fdefpt node)
{
	unsigned index = hash(node->name);
	fdefpt pt = funcDefHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(pt->name, node->name)==0)
		{
			printf("Error type 4 at Line %d: Redefined function \"%s\".\n",node->lineno,node->name);
			return -1;
		}
		pt=pt->next;
	}
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
						while(tp->kind != BASIC && tp->kind != STRUCTURE)
						{
							printf("		array_size: %d\n", tp->array.size);
							tp=tp->array.elem;
						}
						if(tp->kind == BASIC)
						{
							printf("		the para's basic type is %d\n", tp->basic);
						}
						if(tp->kind == STRUCTURE)
						{
							printf("		the para's struct_type is %s\n", pp->struct_name);
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

	sympt pt = symHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(node->name, pt->name)==0)
		{
			printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->lineno, node->name);
			return -1;
		}
		pt=pt->next;
	}

	spt pt2 = structDefHashHead[index];
	while(pt2!=NULL)
	{
		if(strcmp(node->name, pt2->name)==0)
		{
			printf("Error type 3 at Line %d: Redefined variable \"%s\", conflict with struct name.\n",node->lineno, node->name);
			return -1;
		}
		pt2=pt2->next;
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

sympt lookup_sym(tree* root)
{
	sympt sym;
	unsigned index = hash(root->value);
	sympt pt = symHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(root->value,pt->name)==0)
		{
			sym = pt;
			return sym;
		}
		pt = pt->next;
	}
	return NULL;
}

spt lookup_struct(tree* root)
{
	spt str;
	unsigned index = hash(root->struct_name);
	spt pt = structDefHashHead[index];
	while(pt!= NULL)
	{
		if(strcmp(root->struct_name,pt->name)==0)
		{
			str = pt;
			return str;
		}
		pt = pt->next;
	}
	return NULL;
}

fdefpt lookup_func(tree* root)
{
	fdefpt func;
	unsigned index = hash(root->value);
	fdefpt pt = funcDefHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(root->value, pt->name)==0)
		{
			func = pt;
			return func;
		}
		pt = pt->next;
	}
	return NULL;
}

void check_structtable()
{
	for(int i=0; i<hash_size; i++)
	{
		if(structDefHashHead[i]!=NULL)
		{
			spt pt = structDefHashHead[i];
			while(pt!=NULL)
			{
				printf("struct name:%s\n", pt->name);
				FieldList fpt = pt->fieldList;
				while(fpt!=NULL)
				{
					printf("  field name:%s ",fpt->name);
					Type tp = fpt->type;
					printf("field type: %d\n", tp->kind);
					while(tp->kind != BASIC && tp->kind != STRUCTURE)
					{
						printf("		array_size: %d\n", tp->array.size);
						tp=tp->array.elem;
					}
					if(tp->kind == BASIC)
					{
						printf("		the field's basic type is %d\n", tp->basic);
					}
					if(tp->kind == STRUCTURE)
					{
						if(strlen(fpt->struct_name)!=0)
							printf("		the field's struct_type is %s\n", fpt->struct_name);
						else
						{
							printf("		this is a nested struct\n");
							spt pointer = tp->structDefpt;
							FieldList fl = pointer->fieldList;

							while(fl!=NULL)
							{
								//printf("hello\n");
								//TODO TODO
								printf("			field name:%s\n", fl->name);
								fl = fl->next;
							}
						}
					}
					fpt = fpt->next;
				}
				pt = pt->next;
			}
		}
	}
}

int insert_structTable(spt structpt)
{
	unsigned index = hash(structpt->name);

	spt pt = structDefHashHead[index];
	while(pt!=NULL)
	{
		if(strcmp(structpt->name, pt->name)==0)
		{
			printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", structpt->lineno,structpt->name);
			return -1;
		}
		pt = pt->next;
	}
	if(structDefHashHead[index] == NULL)
	{
		structDefHashHead[index] = structpt;
		structpt->next = NULL;
		return 0;
	}
	else
	{
		structpt->next = structDefHashHead[index];
		structDefHashHead[index] = structpt;
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
				printf("sym_name:%s ", pt->name);
				printf("sym_type:%d\n", pt->type->kind);
/*
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
					*/
				pt=pt->next;
			}
		}
	}
}

void CheckPoint()
{
	for(int i = 0; i<hash_size; i++)
	{
		if(funcDecHashHead[i]!=NULL)
		{
			fdefpt func = funcDecHashHead[i];
			while(func!=NULL)
			{
				tree* node = (tree*)malloc(sizeof(tree));
				strcpy(node->value, func->name);

				fdefpt fundef = lookup_func(node);
				if(fundef == NULL)
				{
					printf("Error type 18 at Line %d: Undefined function \"%s\".\n", func->lineno, func->name);
				}
				else
				{
					if(func->return_type != fundef->return_type)
					{
						printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\".\n", func->lineno, func->name);
						func=func->next;
						continue;
					}
					struct Param* para1 = func->para_list;
					struct Param* para2 = fundef->para_list;
					int para_conflict = 0;
					while(para1 != NULL || para2 != NULL)
					{
						if((para1==NULL && para2!=NULL) || (para1!=NULL && para2==NULL))
						{
							para_conflict = 1;
							break;
						}
						if(para1->type->kind != para2->type->kind)
						{
							para_conflict = 1;
							break;
						}
						else if(para1->type->kind == BASIC)
						{
							if(para1->type->basic != para2->type->basic)
							{
								para_conflict = 1;
								break;
							}
						}
						else if(para1->type->kind == STRUCTURE)
						{
							if(strcmp(para1->struct_name, para2->struct_name)!=0)
							{
								para_conflict = 1;
								break;
							}
						}
						else if(para1->type->kind == ARRAY)
						{
							Type type1 = para1->type;
							Type type2 = para2->type;
							int type_error = 0;
							while((type1->kind != BASIC && type1->kind != STRUCTURE) || (type2->kind !=BASIC && type2->kind != STRUCTURE))
							{
								if(((type1->kind==BASIC || type1->kind == STRUCTURE) && (type2->kind!=BASIC && type2->kind!=STRUCTURE)) || ((type2->kind==BASIC || type2->kind == STRUCTURE) && (type1->kind!=BASIC && type1->kind!=STRUCTURE)))
								{
									type_error = 1;
									break;
								}
								type1=type1->array.elem;
								type2=type2->array.elem;
							}
							if(type_error == 1)
							{
								para_conflict = 1;
								break;
							}
							else
							{
								if(type1->kind!=type2->kind)
								{
									para_conflict = 1;
									break;
								}
							}
						}
						para1 = para1->next_para;
						para2 = para2->next_para;
					}
					if(para_conflict == 1)
					{
						printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\", conflict with function definition.\n", func->lineno, func->name);
						func = func->next;
						continue;
					}
				}
				func=func->next;
			}
		}
	}
}
