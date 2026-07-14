#include "cJSON.h"
#include "ds/student_app.h"
#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "shell32.lib")


#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#endif


extern Hashmap *initMap(void);
extern Tree initTree(void);
extern Linked_list initLList(void);
extern void save_data_to_file(void);
extern void load_data_from_file(void);

static void server_event_handler(struct mg_connection *c, int ev,
                                 void *ev_data) {
  if (ev != MG_EV_HTTP_MSG)
    return;

  struct mg_http_message *hm = (struct mg_http_message *)ev_data;

  if (mg_match(hm->uri, mg_str("/"), NULL) ||
      mg_match(hm->uri, mg_str("/index.html"), NULL)) {
    struct mg_http_serve_opts opts = {.root_dir = "web"};
    mg_http_serve_dir(c, hm, &opts);
  }

  // 1. 학생 단건 조회 API
  else if (mg_match(hm->uri, mg_str("/api/student/find"), NULL)) {
    char id[30];
    if (mg_http_get_var(&hm->query, "id", id, sizeof(id)) <= 0) {
      mg_http_reply(c, 400, "", "Missing id");
      return;
    }

    Student *std = (Student *)studentMap->get(studentMap, id);
    if (std == NULL) {
      mg_http_reply(c, 404, "", "Student Not Found");
      return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "id", std->id);
    cJSON_AddStringToObject(root, "name", std->name);
    cJSON_AddNumberToObject(root, "math", std->math);
    cJSON_AddNumberToObject(root, "korean", std->korean);
    cJSON_AddNumberToObject(root, "average", std->average);

    char *json = cJSON_PrintUnformatted(root);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
    cJSON_Delete(root);
  }

  // 2. 학생 신규 추가 API
  else if (mg_match(hm->uri, mg_str("/api/student/insert"), NULL) &&
           mg_strcasecmp(hm->method, mg_str("POST")) == 0) {

    cJSON *root = cJSON_ParseWithLength(hm->body.buf, hm->body.len);
    if (root == NULL) {
      mg_http_reply(c, 400, "", "Invalid JSON");
      return;
    }

    insert_student(studentMap, &studentList, &mathTree, &koreanTree,
                   &averageTree, cJSON_GetObjectItem(root, "id")->valuestring,
                   cJSON_GetObjectItem(root, "name")->valuestring,
                   cJSON_GetObjectItem(root, "math")->valueint,
                   cJSON_GetObjectItem(root, "korean")->valueint);
    cJSON_Delete(root);

    save_data_to_file();
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                  "{\"result\":\"success\"}");
  }

  // 3. 학생 정보 수정 API
  else if (mg_match(hm->uri, mg_str("/api/student/update"), NULL) &&
           mg_strcasecmp(hm->method, mg_str("POST")) == 0) {

    cJSON *root = cJSON_ParseWithLength(hm->body.buf, hm->body.len);
    if (root == NULL) {
      mg_http_reply(c, 400, "", "Invalid JSON");
      return;
    }

    edit_student(studentMap, &studentList, &mathTree, &koreanTree, &averageTree,
                 cJSON_GetObjectItem(root, "id")->valuestring,
                 cJSON_GetObjectItem(root, "name")->valuestring,
                 cJSON_GetObjectItem(root, "math")->valueint,
                 cJSON_GetObjectItem(root, "korean")->valueint);
    cJSON_Delete(root);

    save_data_to_file();
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                  "{\"result\":\"success\"}");
  }

  // 4. 학생 삭제 API
  else if (mg_match(hm->uri, mg_str("/api/student/delete"), NULL) &&
           mg_strcasecmp(hm->method, mg_str("POST")) == 0) {

    cJSON *root = cJSON_ParseWithLength(hm->body.buf, hm->body.len);
    if (root == NULL) {
      mg_http_reply(c, 400, "", "Invalid JSON");
      return;
    }

    char *id = cJSON_GetObjectItem(root, "id")->valuestring;
    remove_std(studentMap, &studentList, &mathTree, &koreanTree, &averageTree,
               id);
    cJSON_Delete(root);

    save_data_to_file();
    mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                  "{\"result\":\"success\"}");
  }

  // 5. 점수 범위 검색 API
  else if (mg_match(hm->uri, mg_str("/api/student/range"), NULL)) {
    char type[20], min_str[10], max_str[10];
    int min, max;
    Linked_list *res_list;
    LLNode *node;
    cJSON *arr;
    char *json;

    if (mg_http_get_var(&hm->query, "type", type, sizeof(type)) <= 0 ||
        mg_http_get_var(&hm->query, "min", min_str, sizeof(min_str)) <= 0 ||
        mg_http_get_var(&hm->query, "max", max_str, sizeof(max_str)) <= 0) {
      mg_http_reply(c, 400, "", "Missing range parameters");
      return;
    }

    min = atoi(min_str);
    max = atoi(max_str);

    if (strcmp(type, "math") == 0)
      res_list = range_search(&mathTree, min / 10, max / 10);
    else if (strcmp(type, "korean") == 0)
      res_list = range_search(&koreanTree, min / 10, max / 10);
    else
      res_list = range_search(&averageTree, min, max);

    arr = cJSON_CreateArray();
    node = res_list->begin;
    while (node != NULL) {
      Student *std = (Student *)node->data;
      if (std) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddStringToObject(obj, "id", std->id);
        cJSON_AddStringToObject(obj, "name", std->name);
        cJSON_AddNumberToObject(obj, "math", std->math);
        cJSON_AddNumberToObject(obj, "korean", std->korean);
        cJSON_AddNumberToObject(obj, "average", std->average);
        cJSON_AddItemToArray(arr, obj);
      }
      node = node->next;
    }

    // 임시 링크드리스트 노드 메모리 해제
    node = res_list->begin;
    while (node != NULL) {
      LLNode *next = node->next;
      free(node);
      node = next;
    }

    json = cJSON_PrintUnformatted(arr);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
    cJSON_Delete(arr);
  }

  // 6. 상위 N명 검사 API.
  else if (mg_match(hm->uri, mg_str("/api/student/top"), NULL)) {
    char type[20], count_str[10];
    int count;
    Linked_list *res_list;
    LLNode *node;
    cJSON *arr;
    char *json;

    if (mg_http_get_var(&hm->query, "type", type, sizeof(type)) <= 0 ||
        mg_http_get_var(&hm->query, "count", count_str, sizeof(count_str)) <=
            0) {
      mg_http_reply(c, 400, "", "Missing range parameters");
      return;
    }

    count = atoi(count_str);

    if (count <= 0) {
      mg_http_reply(c, 400, "", "Count is too low");
    }

    if (strcmp(type, "math") == 0)
      res_list = top_search(&mathTree, count);
    else if (strcmp(type, "korean") == 0)
      res_list = top_search(&koreanTree, count);
    else
      res_list = top_search(&averageTree, count);

    arr = cJSON_CreateArray();
    node = res_list->begin;
    int rank = 0;
    int prev_score = -1;
    while (node != NULL) {
      Student *std = (Student *)node->data;
      if (strcmp(type, "math") == 0) {
        if (std->math != prev_score) {
          rank++;
          prev_score = std->math;
        }
      } else if (strcmp(type, "korean") == 0) {
        if (std->korean != prev_score) {
          rank++;
          prev_score = std->korean;
        }
      } else {
        if (std->average != prev_score) {
          rank++;
          prev_score = std->average;
        }
      }
      if (std) {
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "rank", rank);
        cJSON_AddStringToObject(obj, "id", std->id);
        cJSON_AddStringToObject(obj, "name", std->name);
        cJSON_AddNumberToObject(obj, "math", std->math);
        cJSON_AddNumberToObject(obj, "korean", std->korean);
        cJSON_AddNumberToObject(obj, "average", std->average);
        cJSON_AddItemToArray(arr, obj);
      }
      node = node->next;
    }

    // 임시 링크드리스트 노드 메모리 해제
    node = res_list->begin;
    while (node != NULL) {
      LLNode *next = node->next;
      free(node);
      node = next;
    }

    json = cJSON_PrintUnformatted(arr);
    mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
    free(json);
    cJSON_Delete(arr);
  }

  else {
    struct mg_http_serve_opts opts = {.root_dir = "web"};
    mg_http_serve_dir(c, hm, &opts);
  }
}

int main(void) {
  struct mg_mgr mgr;

  studentMap = initMap();
  mathTree = initTree();
  koreanTree = initTree();
  averageTree = initTree();
  studentList = initLList();

  load_data_from_file();

  mg_mgr_init(&mgr);

  if (mg_http_listen(&mgr, "http://0.0.0.0:8081", server_event_handler, &mgr) ==
      NULL) {
    printf("Failed to listen on port 8081\n");
    return 1;
  }

  printf("Server successfully started on http://localhost:8081\n");

#ifdef _WIN32
  printf("자동으로 브라우저 창을 연결합니다...\n");
  ShellExecuteA(NULL, "open", "http://localhost:8081", NULL, NULL, SW_SHOWNORMAL);
#endif

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }

  mg_mgr_free(&mgr);
  return 0;
}