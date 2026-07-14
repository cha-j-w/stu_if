#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stdlib.h>

#define STR_SIZE 500
#define ARR_SIZE 100

typedef struct mapNode {
  unsigned char *key;
  int *val;
  struct mapNode *next;
  struct mapNode *parent;
} mapNode;

typedef struct Hashmap {
  int arraysize;
  int size;
  mapNode **arr;
  void (*put)(struct Hashmap *, void *, void *);
  void *(*get)(struct Hashmap *, void *);
  void (*remove)(struct Hashmap *, void *);
  unsigned long (*get_hash)(void *);
  int (*compare_keys)(void *, void *);
  void (*free)(struct Hashmap *);
} Hashmap;

extern unsigned long hashstr(void *key);
extern unsigned long hashint(void *key);
extern int cmpstr(void *fst_p, void *snd_p);
extern int cmpint(void *fst_p, void *snd_p);
extern Hashmap *init_hashmap(unsigned long (*gethash)(void *),
                             int (*comparekeys)(void *, void *));

#endif // !__HASHMAP_H__
