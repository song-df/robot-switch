
#include<time.h>
#include "task_work.h"



extern int errno;
static TaskList *gpTaskList;			// 任务队列

//epoll recv 收到后的回调
static int in_task_woker_callback(TaskInfo * task)
{
    task_list_put(gpTaskList, task);
    
}

int CheckSocketConnected(int sock)
{
    if (sock <= 0)
        return 0;
    struct tcp_info info;
    int len = sizeof(info);
    getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
    return info.tcpi_state == TCP_ESTABLISHED ? 1 :0;
}

// 线程的启动函数
static void * in_task_worker_thread_start(void *arg)
{
    struct sockaddr_in      serverAddr,serverYansheeWrapAddr;
    int                     socketFd,socketYansheeWrapFd;
    int                     isYansheeConnected = 0;
    int i;

    char * last_buffer =(char*)malloc(MAXLINE);
    memset(last_buffer,0,MAXLINE);

    char * buf = (char *)malloc(MAXLINE);
    memset(buf,0,MAXLINE);


    bzero(&serverAddr, sizeof(serverAddr)); /*  全部置零  */
    bzero(&serverYansheeWrapAddr, sizeof(serverYansheeWrapAddr)); /*  全部置零  */

    /* 设置地址相关的属性 */
    serverAddr.sin_family         =   AF_INET;
    serverAddr.sin_addr.s_addr    =   htons(INADDR_ANY);
    serverAddr.sin_port           =   htons(ROBOT_PORT);
    /*  创建套接字  */
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd < 0)
    {
        log_info("Robot socket create error[errno = %d]\n", errno);
        exit(-1);
    }
    else
    {
        log_info("Robot socket create success...\n");
    }

     //设置监听套接字
    if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i))<0)
    {
        log_info("设置监听 socket SO_REUSEADDR 错误:%s\n", strerror(errno));
        close(socketFd);
        exit(-1);
    }

    //设置socket重用
    if(setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(socketFd);
        exit(-1);
    }

    /*  绑定端口  */
    if(bind(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) > 0)
    {
        perror("bind error\n");
        exit(-1);
    }
    else
    {
        log_info("Robot Server bind port %d success...\n", ROBOT_PORT);
    }
    
    /*  开始监听绑定的端口  */
    if(listen(socketFd, LISTEN_QUEUE_SIZE))
    {
        log_info("Robot Server listen error[errno = %d]...\n", errno);
        exit(-1);
    }
    else
    {
        log_info("Robot Server listen success...\n");
    }

    struct sockaddr_in  robotAddr;
    socklen_t           length = sizeof(robotAddr);
    int                 connFd = 0;
    while (1)
    {
        if((connFd = accept(socketFd, (struct sockaddr *)&robotAddr, &length)) < 0)
        {
            continue;
        }
        else
        {
            log_info("Robot accept success!\n");
            break;
        }
    }
    

    TaskInfo *task = NULL;
    while (1)
    {
        if(!CheckSocketConnected(connFd))
        {
            log_info("Robot not online\n");
            connFd = accept(socketFd, (struct sockaddr *)&robotAddr, &length);
            log_info("Robot accept success!\n");
        }
        log_info("waiting cmd ...\n");
        task = task_list_get(gpTaskList);

        if (memcmp(last_buffer,task->pszBuffer,MAXLINE)==0)
        {	
            snprintf(buf,MAXLINE,"当前指令正在执行或与上一个重复,请更换指令");
            send(task->socket, buf, strlen(buf), MSG_NOSIGNAL);
            memset(buf,0,MAXLINE);
            //清除任务
            goto end;
        }
        
        memcpy(last_buffer,task->pszBuffer,MAXLINE);

        log_info("cmd msg:%s\n", task->pszBuffer);
        if (memcmp(task->pszBuffer, "小苹果", 9) == 0)
        {
            log_info("小苹果 太LOW 不跳\n");
            send(task->socket, "小苹果 太LOW 不跳", strlen("小苹果 太LOW 不跳"), MSG_NOSIGNAL);
            goto end;
        }

        //禁止send()函数向系统发送异常消息
        send(connFd, task->pszBuffer, strlen(task->pszBuffer), MSG_NOSIGNAL);
        memset(buf,0,MAXLINE);  

        // 读取机器人返回;
        while (recv(connFd, buf, MAXLINE,0))
        {
            if(strcmp(buf,"start")==0)
            {
                snprintf(buf,MAXLINE-1,"开始执行(%s)动作,还有(%d)个动作没有执行,请稍后",task->pszBuffer,task_list_get_num(gpTaskList));
                send(task->socket, buf, strlen(buf), MSG_NOSIGNAL);
                memset(buf,0,MAXLINE);
                continue;
            }
            else if(strcmp(buf,"done")==0)
            {
                // 获取任务数量，我干完了（）
                snprintf(buf,MAXLINE-1,"(%s)动作执行完成",task->pszBuffer);
                send(task->socket, buf, strlen(buf), MSG_NOSIGNAL);
                memset(buf,0,MAXLINE);
                break;
            }
        }
end:       
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


    if (buf)
    {
        free(buf);
    }
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
    memset(&attr, 0, sizeof(attr));
    //设置线程分离属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, in_task_worker_thread_start, &nHigh);
    epoll_socket_set_callback(in_task_woker_callback);
}

