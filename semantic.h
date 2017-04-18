void semantic_check(tree *root);

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union
	{
		int basic;
		struct { Type elem; int size; } array;	//数组类型信息
		FieldList structure;	//结构体类型的信息是一个链表
	} u;
};

struct FieldList_
{
	char *name;
	Type type;		//type指针
	FieldList next;	//next指针
};
