#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef struct _TaskList TaskList;

TaskList *task_list_new();

int task_list_free(TaskList *p);

void * task_list_get(TaskList *p);

int task_list_put(TaskList *p,  void *d);

int task_list_get_num(TaskList *p);


#ifdef __cplusplus
}
#endif
#endif


