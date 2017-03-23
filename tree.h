typedef struct node
{
	char *name;
	int line;
	struct node *first_child, *next_sibling;
}tree;

tree *insert(char *ch, int num, ...);
void treePrint(tree *root);
