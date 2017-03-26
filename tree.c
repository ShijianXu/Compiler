#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "tree.h"

tree *insert(char *ch, int num, char *value, int lineno, ...)
{
	va_list argp;
	tree *para;
	tree *pre_sibling;

	va_start(argp, lineno);

	tree *parent = (tree *)malloc(sizeof(tree));
	parent->name = ch;
	//必须把内容拷贝下来，不能用指针去指
	strcpy(parent->value, value);

	if(lineno != -1)
		parent->line = lineno;

	if(lineno!=-1 && num==0)
		parent->terminal = 1;
	else
		parent->terminal = 0;

	if(num==0 && lineno==-1)
		parent->empty = 1;
	else
		parent->empty=0;


	if(num == 0)
	{
		parent->first_child = NULL;
		parent->next_sibling = NULL;
		return parent;
	}

	for(int i = 0; i < num; i ++)
	{
		para = va_arg(argp, tree*);
		if(i==0)	//first child
		{
			parent->first_child = para;
			if(lineno==-1)
			{
				parent->line = para->line;
			}
			pre_sibling = para;
		}
		else //a sibling, connected to its former sibling
		{
			pre_sibling->next_sibling = para;
			pre_sibling = para;
		}
	}
	return parent;
}

void treePrint(tree *root, int space)
{
	if(root->empty==0 && root->terminal==0)
	{
		for(int i=0;i<space;i++)
			printf(" ");
		printf("%s (%d)\n", root->name, root->line);
	}
	else if(root->empty==0 && root->terminal==1) 	//terminals
	{
		for(int i=0;i<space;i++)
			printf(" ");
		if(strcmp(root->name, "ID")==0 || strcmp(root->name,"TYPE")==0)
			printf("%s: %s\n", root->name, root->value);
		else if(strcmp(root->name, "INT")==0)
		{
			if(strlen(root->value)==1) 	//0-9,dec
				printf("%s: %d\n", root->name, atoi(root->value));
			else if(root->value[0]=='0' && (root->value[1]=='x' || root->value[1]=='X')) //hex,
			{
				int v=0;
				int len=strlen(root->value);
				for(int i = 2;i<len;i++)
				{
					if(root->value[i]>='0' && root->value[i]<='9')
						v = v*16+(root->value[i]-'0');
					else if(root->value[i]>='a' && root->value[i]<='f')
						v = v*16+((root->value[i]-'a')+10);
					else if(root->value[i]>='A' && root->value[i]<='F')
						v = v*16+((root->value[i]-'A')+10);
				}
				printf("%s: %d\n", root->name, v);
			}
			else if(root->value[0]=='0')
			{
				int v=0;
				int len=strlen(root->value);
				for(int i=1;i<len;i++)
					v = v*8+(root->value[i]-'0');
				printf("%s: %d\n", root->name, v);
			}
			else	//dec
			{
				printf("%s: %d\n", root->name, atoi(root->value));
			}
			//printf("%s: value\n", root->name);
		}
		else if(strcmp(root->name, "FLOAT")==0)
		{
			int len=strlen(root->value);
			int normal = 1;
			for(int i=0;i<len;i++)
			{
				if(root->value[i]=='E' || root->value[i]=='e')
				{
					normal = 0;
					break;
				}
			}
			double v = 0;
			double dec = 0;
			int dot_pos = 0;
			if(normal==1)	//normal float,小数点前后必有数字出现
			{
				int before_dot = 1;
				for(int i=0;i<len;i++)
				{
					if(before_dot)
					{
						if(root->value[i]!='.')
							v=v*10+(root->value[i]-'0');
						else if(root->value[i]=='.')
						{
							dot_pos = i;
							before_dot = 0;
							continue;
						}
					}
					if(!before_dot)
					{
						int num = i-dot_pos;
						double temp = root->value[i]-'0';
						for(int j =0;j<num;j++)
						{
							temp=temp/10;
						}
						dec+=temp;
					}
				}
				v=v+dec;
				printf("%s: %f\n", root->name, v);
			}
			else	//science float
			{
				double v = 0;
				double dec =0;
				int pos = 1;
				int e_pos = 0;
				int dot_pos = 0;
				int len = strlen(root->value);
				int num_10 = 0;
				for(int i=0;i<len;i++)
				{
					if(root->value[i]=='-')
						pos = 0;
					if(root->value[i]=='.')
						dot_pos = i;
					if(root->value[i]=='e' || root->value[i]=='E')
						e_pos = i;
				}

				for(int i=0;i<dot_pos;i++)
				{
					v=v*10+(root->value[i]-'0');
				}
				for(int i=dot_pos+1;i<e_pos;i++)
				{
					int num = i -dot_pos;
					double temp = root->value[i]-'0';
					for(int j=0;j<num;j++)
					{
						temp=temp/10;
					}
					dec+=temp;
				}
				v = v+dec;
				if(pos)
				{
					for(int i=e_pos+1;i<len;i++)
					{
						if(root->value[i]=='+')
							continue;
						num_10=num_10*10+(root->value[i]-'0');
					}
					for(int i=0;i<num_10;i++)
						v = v*10;
				}
				if(!pos)
				{
					for(int i=e_pos+1;i<len;i++)
					{
						if(root->value[i]=='-')
							continue;
						num_10=num_10*10+(root->value[i]-'0');
					}
					for(int i=0;i<num_10;i++)
						v = v/10;
				}
				printf("%s: %f\n", root->name, v);	
			}
		}
		else	//normal terminals
			printf("%s\n",root->name);
	}

	if(root->first_child != NULL)
	{
		treePrint(root->first_child, space+2);
	}

	if(root->next_sibling != NULL)
	{
		treePrint(root->next_sibling, space);
	}
}
