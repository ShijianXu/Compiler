typedef struct node
{
	char *name;
	int line;
	int empty;
	int terminal;
	//int space_num;
	char value[40];
	struct node *first_child, *next_sibling;
}tree;

tree *insert(char *ch, int num, char *value, int lineno, ...);
void treePrint(tree *root, int space);
