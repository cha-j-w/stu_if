#ifndef __MSTRING_H__
typedef unsigned int uint;
#define __MSTRING_H__

#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct string {
  uint16_t size;
  char *c_str;
  void (*copy)(struct string *, char *);
  void (*cat_string)(struct string *, struct string *);
  void (*cat)(struct string *, char *);
  int (*cmp)(struct string, struct string);
  int (*stoi)(struct string);
  int32_t (*index)(struct string, char *);
  int (*endswith)(struct string, char *);
  int (*startswith)(struct string, char *);
  struct string (*rslice)(struct string *, uint16_t, uint16_t);
  void (*slice)(struct string *, uint16_t, uint16_t);
  struct string *(*split)(struct string, char *);
  int (*split_count)(struct string, char *);
  void (*change)(struct string *, char *, char *);
  void (*format)(struct string *, uint, ...);
} string;

extern string init_string(char *str, uint16_t limit);
extern char *itos(int x);
extern int32_t chrp_size(char *chrp);
extern void rev_str(char *og);

#endif // !__MSTRING_H__
