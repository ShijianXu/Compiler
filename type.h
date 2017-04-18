typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_
{
	enum { BASIC, ARRAY, STRUCTURE } kind;
	union
	{
		int basic;
		struct { Type elem; int size; } array;
		FieldList structure;
	} u;
};

struct FIeldList_
{
	char *name;
	Type type;
	FieldList tail;
};
