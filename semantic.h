#ifndef SEMANTIC_H_
#define SEMANTIC_H_

#include "tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FunParaList_* FunParaList;
typedef struct SymTableNode* sympt;
typedef struct FuncDefTableNode* fdefpt;
typedef struct FuncDecTableNode* fdecpt;
typedef struct StructTableNode* spt;
enum basic_type { INT_, FLOAT_};

struct StructTableNode
{
	char name[40];	//结构体名
	Type type;
	spt next;
};

struct FuncDecTableNode
{
	char name[40];
	Type type;
	fdecpt next;
};

struct Param
{
	char name[40];
	Type type;
	struct Param* next_para;
};

struct FuncDefTableNode
{
	char name[40];
	enum basic_type return_type;
	int para_num;
	struct Param* para_list;	
	fdefpt next;
};

struct SymTableNode
{
	char name[40];
	Type type;
	int scope;	//每一个作用域对应一个整数，相应的，该作用域下的符号需要记录自己的作用域
	sympt next;	//串联hash到同一个槽的符号
	sympt scope_next;	//串联同一作用域的符号
};

struct Type_
{
	enum { NONE, BASIC, ARRAY, STRUCTURE} kind;
	union
	{
		enum basic_type basic;	//基本类型，具体是int还是float
		//int a[2][3], 第一分量的type是array,第一分量维度是2，第二分量type是int,第二分量维度是3
		struct
		{ 
			Type elem;	//数组类型
			int size; 	//数组维度
		} array;	//数组类型信息

		FieldList structure;	//结构体类型的信息是一个链表
	};
};

struct FieldList_
{
	char *name;
	Type type;		//type指针
	FieldList next;	//next指针,一个结构体中会有很多元素，一个元素就是一个field
};

#define hash_size 0x3fff
sympt symHashHead[hash_size];
fdefpt funcDefHashHead[hash_size];	//definition
fdecpt funcDecHashHead[hash_size];	//declaration
spt structDefHashHead[hash_size];

void semantic_check(tree *root);
void init_hash_head();
int insert_symtable(sympt node);
int insert_funcDefTable(fdefpt func);
unsigned hash(char *name);

void dfs(tree* root);
fdefpt FunDec(tree *root, Type type_);
void VarList(tree* root, fdefpt fun);
void ParamDec(tree* root, fdefpt fun);
void VarDec(tree* root, Type pre_type, struct Param* para);
Type Specifier(tree* root);

#endif
