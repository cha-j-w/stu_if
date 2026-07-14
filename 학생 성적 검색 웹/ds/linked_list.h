#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <stdarg.h>
#include <stdio.h>
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

extern Linked_list init_linked_list();

#endif // linked_list
