#ifndef __STUDENT_APP_H__
#define __STUDENT_APP_H__

#include "ds/hashmap.h"
#include "ds/linked_list.h"
#include "ds/tree.h"

// 학생 구조체 정의 (MSVC 환경에 맞게 확실하게 주입)
typedef struct Student {
  char id[20];
  char name[30];
  int math;
  int korean;
  int average;
} Student;

// 전역 변수 외부 참조 선언
extern Hashmap *studentMap;
extern Linked_list studentList;
extern Tree mathTree, koreanTree, averageTree;

// 함수 외부 참조 선언
extern void insert_student(Hashmap *map, Linked_list *llist, Tree *mathTree,
                           Tree *koreanTree, Tree *averageTree, char *id,
                           char *name, int math, int korean);
extern void remove_std(Hashmap *map, Linked_list *llist, Tree *mathTree,
                       Tree *koreanTree, Tree *averageTree, char *id);
extern void edit_student(Hashmap *map, Linked_list *llist, Tree *mathTree,
                         Tree *koreanTree, Tree *averageTree, char *id,
                         char *name, int math, int korean);
extern Linked_list *range_search(Tree *tree, int min, int max);
extern Linked_list *top_search(Tree *tree, int count);

#endif
