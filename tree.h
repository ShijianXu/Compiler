typedef struct node
{
	char *name;
	int line;
	struct node *first_child, *next_sibling;
}tree;

tree *insertTree(char *ch, tree *parent, tree *pre_sibling);
tree *create(char *ch);
