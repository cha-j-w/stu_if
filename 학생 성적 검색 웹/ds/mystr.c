#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

string init_string(char *str, uint16_t limit);

static int power(int x, int y) {
  int res = 1;
  for (int i = 0; i < y; i++) {
    res *= x;
  }
  return res;
}

static int32_t return_lowest(int count, ...) {
  va_list ap;
  va_start(ap, count);
  int32_t lowest = INT_MAX;
  for (int i = 0; i < count; i++) {
    int32_t arg = va_arg(ap, int);
    if (arg != -1 && arg < lowest) {
      lowest = arg;
    }
  }
  return lowest == INT_MAX ? -1 : lowest;
}

int32_t chrp_size(char *chrp) {
  int32_t size = 0;
  for (int32_t i = 0; i < INT32_MAX; i++) {
    if (chrp[i]) {
      size++;
    } else {
      break;
    }
  }
  return size;
}

void rev_str(char *og) {
  uint osize = (uint)chrp_size(og);
  for (uint i = 0; 2 * i < osize; i++) {
    char backup = og[i];
    og[i] = og[osize - i - 1];
    og[osize - i - 1] = backup;
  }
}

char *itos(int x) {
  int digits = 0;
  int cpx = x;
  while (cpx >= 10) {
    cpx /= 10;
    digits++;
  }
  char *str = (char *)malloc((digits + 2) * sizeof(char));
  for (int i = digits; i >= 0; i--) {
    str[i] = (x % 10) + '0';
    x /= 10;
  }
  str[digits + 1] = 0;
  return str;
}

static void _copy(string *str, char *og) {
  uint16_t nsize = chrp_size(og);
  str->size = nsize;
  char *new_cstr = (char *)malloc((nsize + 1) * sizeof(char));
  for (int i = 0; i < nsize; i++) {
    new_cstr[i] = og[i];
  }
  new_cstr[nsize] = 0;
  free(str->c_str);
  str->c_str = new_cstr;
}

static int _cmp(string str, string str2) {
  if (str.size == str2.size) {
    while (*str.c_str && (*str.c_str == *str2.c_str)) {
      str.c_str++;
      str2.c_str++;
    }
    return *str.c_str == *str2.c_str;
  } else {
    return 0;
  }
}

static void _cat_str(string *str, string *str2) {
  uint32_t new_size = str->size + str2->size;
  if (new_size > UINT16_MAX) {
    return;
  }
  char *new_cstr = (char *)malloc((new_size + 1) * sizeof(char));
  for (uint16_t i = 0; i < new_size; i++) {
    new_cstr[i] = i < str->size ? str->c_str[i] : str2->c_str[i - str->size];
  }
  new_cstr[new_size] = 0;
  free(str->c_str);
  str->size = new_size;
  str->c_str = new_cstr;
}

static void _cat(string *str, char *chr) {
  uint32_t new_size = str->size;
  for (uint16_t i = 0; i < UINT16_MAX; i++) {
    if (chr[i]) {
      new_size += 1;
    } else {
      break;
    }
    if (new_size > UINT16_MAX) {
      return;
    }
  }
  char *new_cstr = (char *)malloc((new_size + 1) * sizeof(char));
  for (uint16_t i = 0; i < new_size; i++) {
    new_cstr[i] = i < str->size ? str->c_str[i] : chr[i - str->size];
  }
  new_cstr[new_size] = 0;
  free(str->c_str);
  str->c_str = new_cstr;
  str->size = new_size;
}

static int _stoi(string str) {
  int result = 0;
  for (uint16_t i = 0; i < str.size; i++) {
    char chr = str.c_str[i];
    if (chr < '0' || chr > '9') {
      return result;
    }
    result += (chr - '0') * power(10, str.size - i - 1);
  }
  return result;
}

static int32_t _index(string str, char *target) {
  uint16_t correct = 0;
  uint16_t tsize = (uint16_t)chrp_size(target);
  for (uint16_t i = 0; i < str.size; i++) {
    if (str.c_str[i] == target[correct]) {
      correct++;
      if (correct == tsize) {
        return i - correct + 1;
      }
    } else {
      i = correct > 0 ? i - correct : i;
      correct = 0;
    }
  }
  return -1;
}

static int _endswith(string str, char *target) {
  uint16_t tsize = (uint16_t)chrp_size(target);
  return str.index(str, target) == str.size - tsize + 1;
}

static int _startswith(string str, char *target) {
  return str.index(str, target) == 0;
}

static string _rslice(string *str, uint16_t a, uint16_t b) {
  if (b < a) {
    return (string){0};
  }
  uint16_t nsize = b - a + 1;
  char *new_cstr = (char *)malloc((nsize + 1) * sizeof(char));
  for (uint16_t i = a; i <= b; i++) {
    new_cstr[i - a] = str->c_str[i];
  }
  new_cstr[nsize] = 0;
  return init_string(new_cstr, -1);
}

static void _slice(string *str, uint16_t a, uint16_t b) {
  if (b < a) {
    free(str->c_str);
    str->c_str = NULL;
    str->size = 0;
    return;
  }
  uint16_t nsize = b - a + 1;
  char *new_cstr = (char *)malloc((nsize + 1) * sizeof(char));
  for (uint16_t i = a; i <= b; i++) {
    new_cstr[i - a] = str->c_str[i];
  }
  new_cstr[nsize] = 0;
  free(str->c_str);
  str->c_str = new_cstr;
  str->size = nsize;
}

static string *_split(string str, char *pattern) {
  uint16_t psize = (uint16_t)chrp_size(pattern);
  if (psize == 0) {
    return NULL;
  }
  uint16_t nsize = chrp_size(str.c_str);
  str.size = nsize;
  char *new_cstr = (char *)malloc((nsize + 1) * sizeof(char));
  for (int i = 0; i < nsize; i++) {
    new_cstr[i] = str.c_str[i];
  }
  new_cstr[nsize] = 0;
  str.c_str = new_cstr;
  int32_t idx = str.index(str, pattern);
  string *res = NULL;
  uint length = 0;
  uint count = 0;
  while (idx != -1) {
    if (length == 0) {
      res = (string *)malloc(sizeof(string));
      length = 1;
    } else if (length - 1 <= count) {
      uint nlength = length * 2;
      res = (string *)realloc(res, nlength * sizeof(string));
      length = nlength;
      for (uint i = nlength / 2; i < nlength; i++) {
        res[i] = (string){0};
      }
    }
    res[count] = str.rslice(&str, 0, idx - 1);
    str.slice(&str, idx + psize, str.size - 1);
    idx = str.index(str, pattern);
    count++;
  }
  if (str.size > 0) {
    res[count] = init_string(str.c_str, -1);
    count++;
  }
  return res;
}

int _split_count(string str, char *pattern) {
  int cnt = 0;
  uint16_t psize = (uint16_t)chrp_size(pattern);
  if (psize == 0) {
    return -1;
  }
  string s = str;
  int idx = s.index(s, pattern);

  while (idx != -1) {
    cnt++;
    s.c_str += idx + psize;
    s.size -= idx + psize;
    idx = s.index(s, pattern);
  }
  if (s.size > 0) {
    cnt++;
  }

  return cnt;
}

static void _change(string *str, char *target, char *replace) {
  int32_t idx = str->index(*str, target);
  if (idx != -1) {
    uint16_t tgsize = (uint16_t)chrp_size(target);
    uint16_t rpsize = (uint16_t)chrp_size(replace);
    uint16_t newsize = str->size - tgsize + rpsize;
    string left = str->rslice(str, 0, idx - 1);
    string middle = init_string(replace, -1);
    string right = str->rslice(str, idx + tgsize, str->size - 1);
    left.cat_string(&left, &middle);
    left.cat_string(&left, &right);
    str->copy(str, left.c_str);
  }
}

static void _format(string *str, uint count, ...) {
  va_list ap;

  va_start(ap, count);
  for (uint i = 0; i < count; i++) {
    int32_t intfmt = str->index(*str, "%d");
    // int32_t fltfmt = str->index(*str, "%lf");
    int32_t chrfmt = str->index(*str, "%c");
    int32_t strfmt = str->index(*str, "%s");

    if (intfmt != -1 || chrfmt != -1 || strfmt != -1) {
      int32_t lowest = return_lowest(3, intfmt, chrfmt, strfmt);
      if (lowest == intfmt) {
        str->change(str, "%d", itos(va_arg(ap, int)));
      } else if (lowest == chrfmt) {
        char *ch = (char *)malloc(2 * sizeof(char));
        ch[0] = (char)va_arg(ap, int);
        ch[1] = 0;
        str->change(str, "%c", ch);
      } else if (lowest == strfmt) {
        char *chstr = va_arg(ap, char *);
        str->change(str, "%s", chstr);
      }
    }
  }

  va_end(ap);
}

string init_string(char *str, uint16_t limit) {
  string new_string;

  for (uint16_t i = 0; i < (limit ? limit : UINT16_MAX); i++) {
    if (!str[i]) {
      new_string.size = i;
      break;
    }
  }

  new_string.c_str = (char *)malloc((new_string.size + 1) * sizeof(char));
  for (int i = 0; i < new_string.size; i++) {
    new_string.c_str[i] = str[i];
  }
  new_string.c_str[new_string.size] = 0;
  new_string.copy = _copy;
  new_string.cat_string = _cat_str;
  new_string.cmp = _cmp;
  new_string.stoi = _stoi;
  new_string.cat = _cat;
  new_string.index = _index;
  new_string.startswith = _startswith;
  new_string.endswith = _endswith;
  new_string.rslice = _rslice;
  new_string.slice = _slice;
  new_string.split = _split;
  new_string.split_count = _split_count;
  new_string.change = _change;
  new_string.format = _format;

  return new_string;
}
