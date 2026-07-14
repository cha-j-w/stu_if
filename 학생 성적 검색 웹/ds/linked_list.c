#include <stdarg.h>
#include <stdlib.h>

typedef struct LLNODE {
  void *data;
  struct LLNODE *next;
  struct LLNODE *parent;
} LLNode;

typedef struct LINKED_LIST {
  LLNode *begin;
  LLNode *last;
  LLNode *(*insert)(struct LINKED_LIST *, LLNode *, void *);
  void (*remove)(struct LINKED_LIST *, LLNode *);
} Linked_list;

static LLNode *_insert(Linked_list *list, LLNode *node, void *data) {
  LLNode *new = (LLNode *)malloc(sizeof(LLNode));
  new->data = data;
  new->next = NULL;
  new->parent = NULL;
  if (list->begin == NULL && list->last == NULL) {
    list->begin = new;
    list->last = new;
  } else {
    if (node->next == NULL) {
      node->next = new;
      new->parent = node;
      list->last = new;
    } else {
      LLNode *next = node->next;
      node->next = new;
      new->parent = node;
      new->next = next;
      next->parent = new;
    }
  }
  return new;
}

static void _remove(Linked_list *list, LLNode *node) {
  LLNode *parent = node->parent;
  LLNode *next = node->next;
  if (parent != NULL) {
    parent->next = next;
  } else {
    list->begin = next;
  }
  if (next != NULL) {
    next->parent = parent;
  } else {
    list->last = parent;
  }
  free(node);
}

Linked_list init_linked_list() {
  Linked_list list;
  list.begin = NULL;
  list.last = NULL;
  list.insert = _insert;
  list.remove = _remove;

  return list;
}
