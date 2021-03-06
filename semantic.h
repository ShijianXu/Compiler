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
enum EXP_TYPE {_NONE, _FUNC, _STRUCTURE, _ARRAY, _INT, _FLOAT};

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
	int firstCallVarDec;
	//用于标示当前处理的是什么地方的变量
	enum {GLO_VAR, FUN_DEC, FUN_DEF, FUN_BODY, STR_DEF } node_kind;
	char struct_name[40];	//当一个变量的类型为结构体时有用
	
	//标示变量的作用域，同一作用域内的变量scope值相同
	int scope;

	Type type;
	//FUN_BODY GLO_VAR中变量
	sympt syms;
	
	int fun_definition;
	//FUN_DEC参数
	fdefpt func;
	//FUN_DEC中的某一个参数，之后插入func的参数链表中
	struct Param* para;

	//STR_DEF 中的变量
	spt stpt;
	//fieldpt记录结构体的某一个域，之后插入结构体的filedList中
	FieldList field;

	enum EXP_TYPE exp_type; //args()调用exp()时用于记录相关信息返回
	int array_dim;
	enum EXP_TYPE array_basic_type;
	struct ArgsType* args;
	int isNum;
	Type return_type;

	//---中间代码生成需要继承的一些属性---
	int array_ic;
	int array_dim_value;
	sympt arr_sym;
}tree;

tree *insert(char *ch, int num, char *value, int lineno, ...);
void treePrint(tree *root, int space);


//#####################################

struct ArgsType
{
	enum EXP_TYPE et;
	int array_dim;
	struct ArgsType* next;
};

struct StructTableNode
{
	int lineno;
	char name[40];	//结构体名
	enum {Definition, Declaration} kind;
	FieldList fieldList;
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
	char struct_name[40];
	struct Param* next_para;
};

struct FuncDefTableNode
{
	int lineno;
	char name[40];
	enum basic_type return_type;
	int para_num;
	struct Param* para_list;
	fdefpt next;
};

struct SymTableNode
{
	int lineno;
	char name[40];
	Type type;
	char struct_name[40];
	int scope;	//每一个作用域对应一个整数，相应的，该作用域下的符号需要记录自己的作用域
	sympt next;	//串联hash到同一个槽的符号
	sympt scope_next;	//串联同一作用域的符号
};

struct Type_
{
	enum { NONE, FUNCTION, BASIC, ARRAY, STRUCTURE} kind;
	union
	{
		enum basic_type basic;	//基本类型，具体是int还是float
		//int a[2][3], 第一分量的type是array,第一分量维度是2，第二分量type是int,第二分量维度是3
		struct
		{ 
			Type elem;	//数组类型
			int size; 	//数组维度
		} array;	//数组类型信息

		//FieldList fieldList;	//结构体类型的信息是一个链表
		spt structDefpt;
	};
};

struct FieldList_
{
	char name[40];
	char struct_name[40];
	Type type;		//type指针
	FieldList next;	//next指针,一个结构体中会有很多元素，一个元素就是一个field
};

#define hash_size 0x3fff
sympt symHashHead[hash_size];
fdefpt funcDefHashHead[hash_size];	//definition
fdefpt funcDecHashHead[hash_size];	//declaration
spt structDefHashHead[hash_size];

void semantic_check(tree *root);
void init_hash_head();
int insert_symtable(sympt node);
int insert_funcDefTable(fdefpt func);
int insert_funcDecTable(fdefpt func);
int insert_structTable(spt structpt);
unsigned hash(char *name);

void dfs(tree* root, int space);
void FunDec(tree *root);
void VarList(tree* root);
void ParamDec(tree* root);
void VarDec(tree* root);
Type Specifier(tree* root);
void check_symtable();
void check_functable();
void check_structtable();
void StructSpecifier(tree* root);
void DefList(tree* root);
void Def(tree* root);
void CompSt(tree* root);
void DecList(tree* root);
void Dec(tree* root);
void ExtDecList(tree* root);
void CompSt(tree* root);
void StmtList(tree* root);
void Stmt(tree* root);
void Exp(tree* root);
int Args(tree* root, int num);
void CheckPoint();

void insertReadWrite();

sympt lookup_sym(tree* root);
fdefpt lookup_func(tree* root);
spt lookup_struct(tree* root);
#endif
