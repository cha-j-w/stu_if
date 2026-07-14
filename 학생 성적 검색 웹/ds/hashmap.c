#include <stdint.h>
#include <stdlib.h>

typedef struct mapNode {
  void *key;
  void *val;
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

Hashmap *init_hashmap(unsigned long (*gethash)(void *),
                      int (*comparekeys)(void *, void *));

unsigned long hashstr(void *key) {
  unsigned char *str = (unsigned char *)key;
  unsigned long hash = 5381;

  int c = 1;
  while (c) {
    c = *str++;
    if (c == 0) {
      break;
    }
    hash = (hash << 5) + hash + c;
  }
  return hash;
}

unsigned long hashint(void *key) {
  unsigned int x = *(unsigned int *)key;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}

int cmpstr(void *fst_p, void *snd_p) {
  unsigned char *fst = (unsigned char *)fst_p;
  unsigned char *snd = (unsigned char *)snd_p;
  while (*fst && (*fst == *snd)) {
    fst++;
    snd++;
  }
  return *fst == *snd;
}

int cmpint(void *fst_p, void *snd_p) { return *(int *)fst_p == *(int *)snd_p; }

static mapNode *createNode(void *key, void *val, mapNode *prev) {
  mapNode *node = (mapNode *)malloc(sizeof(mapNode));
  node->key = key;
  node->val = val;
  node->next = NULL;
  if (prev != NULL) {
    prev->next = node;
    node->parent = prev;
  } else {
    node->parent = NULL;
  }
  return node;
}

static void rehash(Hashmap *map, int nsize, mapNode **new_arr) {
  int old_size = map->arraysize;
  mapNode **old_arr = map->arr;
  for (int i = 0; i < old_size; i++) {
    mapNode *node = old_arr[i];
    while (node != NULL) {
      int nidx = map->get_hash(node->key) % nsize;
      mapNode *next = node->next;
      mapNode *extNode = new_arr[nidx];
      if (extNode != NULL) {
        extNode->parent = node;
        node->next = extNode;
      } else {
        node->next = NULL;
      }
      node->parent = NULL;
      new_arr[nidx] = node;
      node = next;
    }
  }

  free(old_arr);
  map->arraysize = nsize;
  map->arr = new_arr;
}

static void _put(Hashmap *map, void *key, void *val) {
  int idx = map->get_hash(key) % map->arraysize;
  mapNode *existingNode = map->arr[idx];
  mapNode *lastNode = NULL;
  while (existingNode != NULL) {
    if (map->compare_keys(existingNode->key, key)) {
      existingNode->val = val;
      return;
    }
    lastNode = existingNode;
    existingNode = existingNode->next;
  }
  mapNode *node = createNode(key, val, lastNode);
  if (lastNode == NULL) {
    map->arr[idx] = node;
  }
  map->size++;

  if (map->size >= map->arraysize * 0.75) {
    int nsize = map->arraysize * 2;
    mapNode **new_arr = (mapNode **)malloc(nsize * sizeof(mapNode *));

    for (int i = 0; i < nsize; i++) {
      new_arr[i] = NULL;
    }

    rehash(map, nsize, new_arr);
  }
}

static mapNode *get_node_w_key(Hashmap *map, void *key) {
  int idx = map->get_hash(key) % (unsigned long)map->arraysize;
  mapNode *node = map->arr[idx];
  while (node != NULL) {
    if (map->compare_keys(node->key, key)) {
      return node;
    }
    node = node->next;
  }
  return NULL;
}

static void *_get(Hashmap *map, void *key) {
  mapNode *node = get_node_w_key(map, key);
  return node == NULL ? NULL : node->val;
}

static void _remove(Hashmap *map, void *key) {
  int idx = map->get_hash(key) % map->arraysize;
  mapNode *node = get_node_w_key(map, key);
  if (node != NULL) {
    mapNode *prev = node->parent;
    mapNode *next = node->next;
    if (prev) {
      prev->next = next;
    } else {
      map->arr[idx] = next;
    }
    if (next) {
      next->parent = prev;
    }
    free(node);
    map->size--;
  }
}

static void _free(Hashmap *map) {
  for (int i = 0; i < map->arraysize; i++) {
    mapNode *node = map->arr[i];
    mapNode *next = NULL;
    while (node != NULL) {
      next = node->next;
      free(node);
      node = next;
    }
  }
  free(map);
}

Hashmap *init_hashmap(unsigned long (*gethash)(void *),
                      int (*comparekeys)(void *, void *)) {
  Hashmap *map = (Hashmap *)malloc(sizeof(Hashmap));
  map->arraysize = 100;
  map->size = 0;
  map->arr = (mapNode **)malloc(map->arraysize * sizeof(mapNode *));
  for (int i = 0; i < map->arraysize; i++) {
    map->arr[i] = NULL;
  }
  map->put = _put;
  map->get = _get;
  map->remove = _remove;
  map->free = _free;
  map->get_hash = gethash;
  map->compare_keys = comparekeys;

  return map;
}
