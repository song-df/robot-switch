#include "libtaskList.h"
#include "malloc.h"
#include <pthread.h>

typedef struct _TaskListData  TaskListData;
struct _TaskListData {
	void			*pData;
	TaskListData	*pNext;
};

struct _TaskList 
{
	pthread_mutex_t struMutex;
	pthread_cond_t struCond;
	TaskListData *head;
	TaskListData *tail;	
	int	Num;
};


/**
 * 创建一个任务队列
 */
TaskList *task_list_new()
{
	TaskList *p = (TaskList *)malloc(sizeof(TaskList));

	pthread_mutex_init(&p->struMutex, NULL);
	pthread_cond_init(&p->struCond, NULL);
	p->tail = p->head = NULL;
	p->Num = 0;
	return p;
}

/**
 * 释放一个任务队列
 */
int task_list_FREE(TaskList *p)
{
	TaskListData *list = p->head, *next;
	if(p->Num > 0)
	{
		return -1;
	}
	
	while(list)
	{
		next = list->pNext;
		free(list);
		list = next;
	}
	pthread_cond_destroy(&p->struCond);
	pthread_mutex_destroy(&p->struMutex);
	free(p);
	return 0;
}

/**
 * 获取一个空闲的任务
 */
void * task_list_get(TaskList *task)
{
	TaskListData *list;
	void *data;
	if(!task)
		return NULL;

	pthread_mutex_lock(&task->struMutex);
	// 应该只需判断一个条件
	while(task->Num < 1 || task->head == NULL)
	{
		pthread_cond_wait(&task->struCond, &task->struMutex);
	}
	
	list = task->head;
	task->head = list->pNext;
	if(-- task->Num == 0)
		task->tail = NULL;
	pthread_mutex_unlock(&task->struMutex);
	data = list->pData;
	free(list);
	return data;
}

/**
 * 将一个任务放入到队列
 */
int task_list_put(TaskList *task, void *d)
{
	TaskListData *list;
	if(!task)
		return -1;

	list = (TaskListData *)malloc(sizeof(TaskListData));
	list->pNext = NULL;
	list->pData = d;

	pthread_mutex_lock(&task->struMutex);

	if(task->Num ++ == 0)
	{
		task->head = task->tail = list;
	}
	else
	{
		task->tail->pNext = list;
		task->tail = list;
	}
	pthread_cond_signal(&task->struCond);
	pthread_mutex_unlock(&task->struMutex);
	return 0;
}

/**
 * 获取一个任务队列的数目
 */
int task_list_get_num(TaskList *p)
{
	return p->Num;
}
