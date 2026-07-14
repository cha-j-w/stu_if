#include "ds/hashmap.h"
#include "ds/linked_list.h"
#include "ds/mstring.h"
#include "ds/tree.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

Hashmap *studentMap = NULL;
Linked_list studentList;
Tree mathTree, koreanTree, averageTree;

typedef struct Student {
  char id[20];
  char name[30];
  int math;
  int korean;
  int average;
} Student;

int cmpStudent(Student *des, Student *src) { return cmpstr(des->id, src->id); }

int isStudentAlready(Student *std, Hashmap *map) {
  Student *got_std = map->get(map, std->id);
  if (got_std) {
    return cmpstr(got_std->id, std->id);
  } else {
    return 0;
  }
}

int insert_student(Hashmap *map, Linked_list *llist, Tree *mathTree,
                   Tree *koreanTree, Tree *averageTree, char *id, char *name,
                   int math, int korean) {
  Student *std = (Student *)malloc(sizeof(Student));
  strcpy(std->id, id);
  strcpy(std->name, name);
  std->math = math;
  std->korean = korean;
  std->average = (math + korean) * 5;
  if (!isStudentAlready(std, map)) {
    llist->insert(llist, llist->last, std);
    map->put(map, std->id, std);
    mathTree->insert(mathTree, std->math, std);
    koreanTree->insert(koreanTree, std->korean, std);
    averageTree->insert(averageTree, std->average, std);
    return 0;
  } else {
    free(std);
    return 1;
  }
}

void sync_with_llist(Linked_list *llist, Hashmap *map, Tree *mathTree,
                     Tree *koreanTree, Tree *averageTree) {
  LLNode *node = llist->begin;
  while (node != NULL) {
    Student *std = (Student *)node->data;
    if (std) {
      map->put(map, std->id, std);
      mathTree->insert(mathTree, std->math, std);
      koreanTree->insert(koreanTree, std->korean, std);
      averageTree->insert(averageTree, std->average, std);
      node = node->next;
    } else {
      LLNode *next = node->next;
      llist->remove(llist, node);
      node = next;
    }
  }
}

Student *find_std_by_id(Hashmap *map, char *id) {
  Student *std = map->get(map, id);
  return std;
}

void remove_from_tree(Tree *tree, Student *std, int idx) {
  TreeNode *treenode = tree->find(tree, idx);
  if (treenode == NULL)
    return;
  Linked_list *treeList = treenode->data;
  if (treeList->begin == treeList->last) {
    tree->remove(tree, treenode);
  } else {
    LLNode *treellnode = treeList->begin;
    while (treellnode != NULL) {
      if (cmpStudent(treellnode->data, std)) {
        treeList->remove(treeList, treellnode);
        break;
      }
      treellnode = treellnode->next;
    }
  }
}

int remove_std(Hashmap *map, Linked_list *llist, Tree *mathTree,
               Tree *koreanTree, Tree *averageTree, char *id) {
  Student *std = find_std_by_id(map, id);
  if (std == NULL) {
    return -1;
  }
  LLNode *llnode = llist->begin;
  while (llnode != NULL) {
    if (cmpStudent(llnode->data, std)) {
      llist->remove(llist, llnode);
      break;
    }
    llnode = llnode->next;
  }
  map->remove(map, std->id);
  remove_from_tree(mathTree, std, std->math);
  remove_from_tree(koreanTree, std, std->korean);
  remove_from_tree(averageTree, std, std->average);
  string str = init_string(std->id, -1);
  int deleted_id = str.stoi(str);
  free(std);
  return deleted_id;
}

Hashmap *initMap() { return init_hashmap(hashstr, cmpstr); }
Tree initTree() { return init_tree(0); }
Linked_list initLList() { return init_linked_list(); }

void freellist(Linked_list *llist) {
  LLNode *inner_value = llist->begin;
  while (inner_value != NULL) {
    LLNode *next = inner_value->next;
    free(inner_value);
    inner_value = next;
  }
}

void freeAll(Hashmap *map, Tree *tree, Linked_list *llist) {
  LLNode *inner_value = llist->begin;
  while (inner_value != NULL) {
    LLNode *next = inner_value->next;
    free(inner_value->data);
    free(inner_value);
    inner_value = next;
  }
  TreeNode *treenode = tree->root;
  while (1) {
    if (treenode == NULL) {
      break;
    }
    if (treenode->left != NULL) {
      treenode = treenode->left;
    } else if (treenode->right != NULL) {
      treenode = treenode->right;
    } else {
      if (treenode->parent != NULL) {
        TreeNode *parent = treenode->parent;
        if (treenode->idx < treenode->parent->idx) {
          treenode->parent->left = NULL;
        } else {
          treenode->parent->right = NULL;
        }
        freellist(treenode->data);
        free(treenode->data);
        free(treenode);
        treenode = parent;
      } else {
        freellist(treenode->data);
        free(treenode->data);
        free(treenode);
        break;
      }
    }
  }
  free(map);
}

void edit_student(Hashmap *map, Linked_list *llist, Tree *mathTree,
                  Tree *koreanTree, Tree *averageTree, char *id, char *name,
                  int math, int korean) {
  Student *std = (Student *)map->get(map, id);
  if (std != NULL) {
    remove_std(map, llist, mathTree, koreanTree, averageTree, id);
    insert_student(map, llist, mathTree, koreanTree, averageTree, id, name,
                   math, korean);
  }
}

void range_search_recu(Linked_list *llist, TreeNode *node, int min, int max) {
  if (node == NULL) {
    return;
  }
  TreeNode *left = node->left;
  TreeNode *right = node->right;
  int idx = node->idx;
  if (left != NULL && min <= idx) {
    range_search_recu(llist, left, min, max);
  }
  if (min <= idx && idx <= max) {
    LLNode *student = node->data->begin;
    while (student != NULL) {
      llist->insert(llist, llist->last, student->data);
      student = student->next;
    }
  }
  if (right != NULL && idx <= max) {
    range_search_recu(llist, right, min, max);
  }
}

Linked_list *range_search(Tree *tree, int min, int max) {
  Linked_list *llist = (Linked_list *)malloc(sizeof(Linked_list));
  *llist = initLList();
  range_search_recu(llist, tree->root, min, max);
  return llist;
}

void save_data_to_file() {
  FILE *fp = fopen("students.txt", "w");
  if (fp == NULL)
    return;

  LLNode *node = studentList.begin;
  while (node != NULL) {
    Student *std = (Student *)node->data;
    if (std) {
      fprintf(fp, "%s,%s,%d,%d\n", std->id, std->name, std->math, std->korean);
    }
    node = node->next;
  }
  fclose(fp);
}

void top_search_recu(Linked_list *llist, TreeNode *node, int *curCount) {
  if (node == NULL || *curCount <= 0) {
    return;
  }
  top_search_recu(llist, node->right, curCount);
  LLNode *student = node->data->begin;
  if (*curCount > 0) {
    while (student != NULL) {
      llist->insert(llist, llist->last, student->data);
      student = student->next;
    }
    *curCount -= 1;
  }
  top_search_recu(llist, node->left, curCount);
}

Linked_list *top_search(Tree *tree, int count) {
  Linked_list *llist = (Linked_list *)malloc(sizeof(Linked_list));
  *llist = initLList();
  top_search_recu(llist, tree->root, &count);
  return llist;
}

void load_data_from_file() {
  FILE *fp = fopen("students.txt", "r");
  if (fp == NULL)
    return;

  char id[20], name[30];
  int math, korean;

  while (fscanf(fp, "%[^,],%[^,],%d,%d\n", id, name, &math, &korean) == 4) {
    insert_student(studentMap, &studentList, &mathTree, &koreanTree,
                   &averageTree, id, name, math, korean);
  }
  fclose(fp);
}

int original_main(void) {
  studentMap = initMap();
  mathTree = initTree();
  koreanTree = initTree();
  averageTree = initTree();
  studentList = initLList();
  load_data_from_file();
  return 0;
}
