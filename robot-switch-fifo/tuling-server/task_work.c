
#include<time.h>
#include "task_work.h"
#include "../libtulingApi/libtulingApi.h"



extern int errno;
static TaskList *gpTaskList;			// 任务队列
//epoll recv 收到后的回调
static int in_task_woker_callback(TaskInfo * task)
{
    task_list_put(gpTaskList, task);
}

// 线程的启动函数
static void * in_task_worker_thread_start(void *arg)
{
    
    InitTulingApi();
    TaskInfo *task;
    while (1)
    {
        task = task_list_get(gpTaskList);
        CallTulingApi(task->socket ,TULING_APIKEY, 
                      task->pszBuffer, strlen(task->pszBuffer),
                      task->pszBuffer,MAXLINE-1);
    
        log_info("fd:%d send %s\n",task->socket,task->pszBuffer);
        send(task->socket, task->pszBuffer, strlen(task->pszBuffer), MSG_NOSIGNAL);

//清除任务
        if (task)
        {
            if (task->pszBuffer)
            {
                free(task->pszBuffer);
            }
            free(task);   
        }

    }
    ReleaseTulingApi();
    return NULL;
}


/**
 * 开始任务服务者
 */
int task_worker_start()
{

    pthread_t thread;
    pthread_attr_t attr;
    int	nHigh = 1;

    gpTaskList = task_list_new();
    log_info("task_worker_start \n");

    // 创建服务线程
    memset(&attr, 0, sizeof(attr));
    //设置线程分离属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    for (int i = 0; i < 2; i++)
    {
        pthread_create(&thread, &attr, in_task_worker_thread_start, &nHigh);
    }
    epoll_socket_set_callback(in_task_woker_callback);

}

