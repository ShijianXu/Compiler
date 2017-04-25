#ifndef SEMANTIC_H_
#define SEMANTIC_H_

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FunParaList_* FunParaList;
typedef struct SymTableNode* sympt;
typedef struct FuncDefTableNode* fdefpt;
typedef struct FuncDecTableNode* fdecpt;
typedef struct StructTableNode* spt;
enum basic_type { INT_, FLOAT_};

//####################################
typedef struct node
{
    char *name;
    int child_num;
    int line;
 	int empty;
    int terminal;
    //int space_num;
	char value[40];
	struct node *first_child, *next_sibling;

	//##################
	int structdef;
	Type type;
	struct Param* para;
	spt stpt;
}tree;

tree *insert(char *ch, int num, char *value, int lineno, ...);
void treePrint(tree *root, int space);


//#####################################
struct StructTableNode
{
	char name[40];	//结构体名
	enum {Definition, Declaration} kind;
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

void dfs(tree* root, int space);
fdefpt FunDec(tree *root, Type type_);
void VarList(tree* root, fdefpt fun);
void ParamDec(tree* root, fdefpt fun);
void VarDec(tree* root);
Type Specifier(tree* root);
void check_functable();
spt StructSpecifier(Type type, tree* root);
void DefList(tree* root);
void Def(tree* node);
void CompSt(tree* root);
void DecList(Type type, tree* root);
void Dec(Type type, tree* root);

#endif
