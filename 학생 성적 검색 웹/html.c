#ifndef __STUDENT_APP_H__
#define __STUDENT_APP_H__

#include "ds/hashmap.h"
#include "ds/linked_list.h"
#include "ds/tree.h"

// main.c에 구현되어 있는 학생 구조체 정의 복사
typedef struct Student {
  char id[20];
  char name[30];
  int math;
  int korean;
  int average;
} Student;

// main.c에 정의된 전역 변수들을 외부에서 사용하겠다고 선언
extern Hashmap *studentMap;
extern Linked_list studentList;
extern Tree mathTree, koreanTree, averageTree;

// main.c에 있는 함수들을 외부에서 쓰기 위한 선언
extern void insert_student(Hashmap *map, Linked_list *llist, Tree *mathTree, Tree *koreanTree, Tree *averageTree, char *id, char *name, int math, int korean);
extern void remove_std(Hashmap *map, Linked_list *llist, Tree *mathTree, Tree *koreanTree, Tree *averageTree, char *id);
extern void edit_student(Hashmap *map, Linked_list *llist, Tree *mathTree, Tree *koreanTree, Tree *averageTree, char *id, char *name, int math, int korean);
extern void range_search(Linked_list *llist, Tree *tree, int min, int max);

#endif