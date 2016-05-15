#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

typedef struct
{
    char *data;
    struct TreeNode *left, *right;
    unsigned char height;
}TreeNode;

typedef struct
{
    TreeNode *root;
    int       length;
}Tree;

Tree* tree_init();
void tree_free(Tree *tree, void(*free_data)(char *data));

void tree_add(Tree *tree, char *data, char(*comparision)(char *, char *));
char* tree_find(Tree *tree, char *data, int(*comparision)(char *s1, char *s2));

void tree_print(Tree *tree, void (*print_data)(char *data));

#endif // TREE_H_INCLUDED
