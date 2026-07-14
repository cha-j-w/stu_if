#include "linked_list.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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

static TreeNode *createNode(int idx, void *data) {
  TreeNode *new = (TreeNode *)malloc(sizeof(TreeNode));
  Linked_list *llist = (Linked_list *)malloc(sizeof(Linked_list));
  *llist = init_linked_list();
  new->idx = idx;
  new->height = 0;
  new->data = llist;
  new->parent = NULL;
  new->left = NULL;
  new->right = NULL;
  llist->insert(llist, llist->last, data);

  return new;
}

static int tree_max(int a, int b) { return a > b ? a : b; }

// use when ll
static TreeNode *_rotate_ll(Tree *tree, TreeNode *node) {
  if (node == NULL) {
    return NULL;
  }
  TreeNode *parent = node->parent;
  if (node->right != NULL) {
    node->right->parent = parent;
  }
  parent->left = node->right;
  node->right = parent;
  node->parent = parent->parent;
  if (parent->parent != NULL) {
    TreeNode *super = parent->parent;
    if (super->left == parent) {
      parent->parent->left = node;
    } else {
      parent->parent->right = node;
    }
  }
  parent->parent = node;
  if (tree->root == parent) {
    tree->root = node;
  }
  int pl = parent->left != NULL ? parent->left->height : -1;
  int pr = parent->right != NULL ? parent->right->height : -1;
  parent->height = 1 + tree_max(pl, pr);

  int nl = node->left != NULL ? node->left->height : -1;
  int nr = node->right != NULL ? node->right->height : -1;
  node->height = 1 + tree_max(nl, nr);

  return node;
}

// use when rr
static TreeNode *_rotate_rr(Tree *tree, TreeNode *node) {
  if (node == NULL) {
    return NULL;
  }
  TreeNode *parent = node->parent;
  if (node->left != NULL) {
    node->left->parent = parent;
  }
  parent->right = node->left;
  node->left = parent;
  node->parent = parent->parent;
  if (parent->parent != NULL) {
    TreeNode *super = parent->parent;
    if (super->right == parent) {
      parent->parent->right = node;
    } else {
      parent->parent->left = node;
    }
  }
  parent->parent = node;
  if (tree->root == parent) {
    tree->root = node;
  }
  int pl = parent->left != NULL ? parent->left->height : -1;
  int pr = parent->right != NULL ? parent->right->height : -1;
  parent->height = 1 + tree_max(pl, pr);

  int nl = node->left != NULL ? node->left->height : -1;
  int nr = node->right != NULL ? node->right->height : -1;
  node->height = 1 + tree_max(nl, nr);
  return node;
}

static TreeNode *_rotate_lr(Tree *tree, TreeNode *node) {
  _rotate_rr(tree, node->right);
  return _rotate_ll(tree, node->parent);
}

static TreeNode *_rotate_rl(Tree *tree, TreeNode *node) {
  _rotate_ll(tree, node->left);
  return _rotate_rr(tree, node->parent);
}

static int _replace_nodes(Tree *tree, TreeNode *node, int inserted_idx) {
  TreeNode *parent = node->parent;

  if (parent == NULL) {
    return 1;
  }
  int lheight = parent->left != NULL ? parent->left->height : -1;
  int rheight = parent->right != NULL ? parent->right->height : -1;
  parent->height = 1 + tree_max(lheight, rheight);

  int bf = lheight - rheight;

  if (bf > 1 && inserted_idx < parent->left->idx) {
    node = _rotate_ll(tree, node);
  } else if (bf < -1 && inserted_idx > parent->right->idx) {
    node = _rotate_rr(tree, node);
  } else if (bf > 1 && inserted_idx > parent->left->idx) {
    node = _rotate_lr(tree, node);
  } else if (bf < -1 && inserted_idx < parent->right->idx) {
    node = _rotate_rl(tree, node);
  }

  return node->parent != parent;
}

static void print_tree_recu(Tree *tree, TreeNode *node, int isleft,
                            char *prefix) {
  if (node == NULL) {
    return;
  }
  char pf[512];
  snprintf(pf, 512, "%s%s", prefix, isleft ? "|   " : "    ");
  print_tree_recu(tree, node->right, 0, pf);
  if (node->parent != NULL) {
    printf("%s%s", prefix, isleft ? "\\-- " : "/-- ");
  }
  printf("%d", node->idx);
  printf("%s", node->height == tree->root->height ? "--|\n" : "\n");
  snprintf(pf, 512, "%s%s", prefix, isleft ? "    " : "|   ");
  pf[0] = ' ';
  print_tree_recu(tree, node->left, 1, pf);
}

static void _print_tree(Tree *tree) {
  print_tree_recu(tree, tree->root, 0, "");
}

static TreeNode *_insert(Tree *tree, int idx, void *data) {
  TreeNode *new = createNode(idx, data);
  TreeNode *cur = tree->root;
  if (cur == NULL) {
    tree->root = new;
    return new;
  }
  if (cur->idx == new->idx) {
    free(new);
    cur->data->insert(cur->data, cur->data->last, data);
    return cur;
  }
  while (1) {
    int smaller = idx < cur->idx;
    TreeNode *ncur = smaller ? cur->left : cur->right;
    if (ncur == NULL) {
      if (idx == cur->idx) {
        free(new);
        cur->data->insert(cur->data, cur->data->last, data);
        return cur;
      }
      if (smaller) {
        cur->left = new;
      } else {
        cur->right = new;
      }
      break;
    } else {
      cur = ncur;
    }
  }
  new->parent = cur;

  TreeNode *for_rotation = new;
  while (for_rotation != NULL) {
    int rotated = _replace_nodes(tree, for_rotation, new->idx);
    for_rotation = for_rotation->parent;
    if (rotated == 1) {
      break;
    }
  }
  return new;
}

static TreeNode *find_right_min(TreeNode *node) {
  TreeNode *right = node->right;
  TreeNode *current = right->left;
  if (current == NULL) {
    return right;
  }
  while (current->left != NULL) {
    current = current->left;
  }
  return current;
}

static void freeLlist(Linked_list *llist) {
  LLNode *inner_value = llist->begin;
  while (inner_value != NULL) {
    LLNode *next = inner_value->next;
    free(inner_value);
    inner_value = next;
  }
}

static void _remove(Tree *tree, TreeNode *node) {
  TreeNode *left = node->left;
  TreeNode *right = node->right;
  TreeNode *remain = left;
  TreeNode *parent = node->parent;
  TreeNode *tparent = parent;
  int isRoot = tree->root == node;
  int isLeft = isRoot ? 1 : parent->idx > node->idx;
  int child_count =
      left != NULL ? (right != NULL ? 2 : 1) : (right != NULL ? 1 : 0);
  remain = left != NULL ? left : right;
  switch (child_count) {
  case 0: {
    if (isRoot) {
      tree->root = NULL;
      freeLlist(node->data);
      free(node->data);
      free(node);
      return;
    }
    if (isLeft) {
      parent->left = NULL;
    } else {
      parent->right = NULL;
    }
    freeLlist(node->data);
    free(node->data);
    free(node);
    break;
  }
  case 1: {
    if (isRoot) {
      tree->root = remain;
      freeLlist(node->data);
      free(node->data);
      remain->parent = NULL;
      free(node);
      return;
    }
    if (isLeft) {
      parent->left = remain;
    } else {
      parent->right = remain;
    }
    remain->parent = parent;
    freeLlist(node->data);
    free(node->data);
    free(node);
    break;
  }
  case 2: {
    if (right != NULL) {
      TreeNode *min = find_right_min(node);
      node->idx = min->idx;

      freeLlist(node->data);
      free(node->data);
      node->data = min->data;
      if (min->parent == node) {
        node->right = min->right;
      } else {
        min->parent->left = min->right;
      }
      if (min->right != NULL) {
        min->right->parent = min->parent;
      }
      tparent = min->parent;
      free(min);
    }
    break;
  }
  }
  TreeNode *for_rotation = tparent;
  while (for_rotation != NULL) {
    int rotated = _replace_nodes(tree, for_rotation, tparent->idx);
    for_rotation = for_rotation->parent;
    if (rotated == 1) {
      break;
    }
  }
}

static TreeNode *_find_recu(Tree *tree, TreeNode *node, int idx) {
  if (node == NULL) {
    return NULL;
  }
  if (idx == node->idx) {
    return node;
  }
  if (idx < node->idx) {
    return _find_recu(tree, node->left, idx);
  } else {
    return _find_recu(tree, node->right, idx);
  }
}

static TreeNode *_find(Tree *tree, int idx) {
  return _find_recu(tree, tree->root, idx);
}

Tree init_tree(int count, ...) {
  Tree tree;
  tree.root = NULL;
  tree.insert = _insert;
  tree.remove = _remove;
  tree.print = _print_tree;
  tree.find = _find;

  va_list ap;
  va_start(ap, count);

  for (int i = 0; i < count; i++) {
    tree.insert(&tree, va_arg(ap, int), 0);
  }
  va_end(ap);

  return tree;
}
