#ifndef __TREE_H__
#define __TREE_H__

#include "linked_list.h"
typedef struct TreeNode {
  int idx;
  Linked_list *data;
  int height;
  struct TreeNode *parent;
  struct TreeNode *left;
  struct TreeNode *right;
} TreeNode;

typedef struct Tree {
  TreeNode *root;
  TreeNode *(*insert)(struct Tree *, int, void *);
  TreeNode *(*find)(struct Tree *, int);
  void (*print)(struct Tree *);
  void (*remove)(struct Tree *, TreeNode *);
} Tree;

extern Tree init_tree(int count, ...);

#endif // !__TREE_H__
