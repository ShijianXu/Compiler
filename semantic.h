#include "tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FunParaList_* FunParaList;
typedef struct SymTableNode* sympt;
struct StructTableNode
{
	char *name;	//结构体名
	Type type;
};

struct FuncTableNode
{
	char *name;
	Type type;
};

struct SymTableNode
{
	char *name;
	Type type;
	int scope;	//每一个作用域对应一个整数，相应的，该作用域下的符号需要记录自己的作用域
	sympt next;	//串联hash到同一个槽的符号
	sympt scope_next;	//串联同一作用域的符号
};

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
	union
	{
		int basic;	//基本类型，具体是int还是float
		//int a[2][3], 第一分量的type是array,第一分量维度是2，第二分量type是int,第二分量维度是3
		struct
		{ 
			Type elem;	//数组类型
			int size; 	//数组维度
		} array;	//数组类型信息

		FieldList structure;	//结构体类型的信息是一个链表
		struct
		{
			int para_num;	//函数参数个数
			Type ret_type;	//函数返回值类型
			FunParaList pl;	//函数参数列表
		} func;
	};
};

struct FunParaList_
{
	char *name;	//形参名
	Type type;	//形参类型
	FunParaList next; //下一个形参
};

struct FieldList_
{
	char *name;
	Type type;		//type指针
	FieldList next;	//next指针,一个结构体中会有很多元素，一个元素就是一个field
};

#define hash_size 0x3fff
sympt hash_head[hash_size];
void semantic_check(tree *root);
void init_hash_head();
int insert_symtable(sympt node);
unsigned hash(char *name);
