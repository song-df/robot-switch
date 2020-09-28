#include "epoll_socket.h"

extern int errno;
static char* buf = NULL;                            //socket读取的数据
static OnRecvPackageCallback gCallBackFunc = NULL;
//设置回调函数
int epoll_socket_set_callback(OnRecvPackageCallback callback)
{
    gCallBackFunc = callback;

    return 0;
}

// 内部使用主线程
static void * epoll_main_thread_start(void *arg)
{
    
    int i = 1,epoll_Event_Num;
    
    buf = (char *)malloc(MAXLINE);
    memset(buf, 0, MAXLINE);

    struct epoll_event event, *pEvents;
    pEvents = malloc(EPOLL_MAX_EVENTS * sizeof(event));
    memset(pEvents,0,EPOLL_MAX_EVENTS * sizeof(event));

    //初始化全局的Socket数组,存放的是所有连接的Socket信息
    pthread_mutex_lock(&gstruMutex);
    gnSocketArrayNum = MIN_SOCKET_Array_NUM;
    gpSocketArray = (SocketInfo **)malloc(sizeof(SocketInfo*) * gnSocketArrayNum);
    for(i = 0; i < gnSocketArrayNum; i ++)
    {
        gpSocketArray[i] = NULL;
    }
    pthread_mutex_unlock(&gstruMutex);

    pthread_detach(pthread_self());

    while(1)
    {
        /* 等待有事件发生 */
        epoll_Event_Num = epoll_wait(gnEpollSocket,
                                     pEvents,
                                     EPOLL_MAX_EVENTS, -1);
        if (epoll_Event_Num == -1)
        {
            log_info("epoll_wait error:%s\n", strerror(errno));
            continue;
        }
        /* 处理所有事件 */
        for (i = 0; i < epoll_Event_Num; ++i)
        {
            //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口,建立新的连接
            if (pEvents[i].data.fd == gnListenSocket)
            {
                int ClientFd;
                struct sockaddr_in Client_Addr;
                socklen_t socklen = sizeof(struct sockaddr_in);

                //建立一个连接
                ClientFd = accept(gnListenSocket, (struct sockaddr *)&Client_Addr,&socklen);

                if(ClientFd < 0)
                {
                    log_info("接收客户端连接失败[%s]\n", strerror(errno));
                    continue;
                }
                else
                {
                    log_info("接收客户端连接成功\n");
                }
                
                if (acceptCount >= EPOLL_MAX_EVENTS)
                {
                    log_info("连接数超过上限:%d\n",EPOLL_MAX_EVENTS);
                    close(ClientFd);
                    continue;
                }

                /*设置socket,连接保活,缓冲区大小*/
                set_Socket(ClientFd,socklen,SOCKET_CACHE_SIZE);

                event.events = EPOLL_MOAD ;//设置epoll LT模式
                event.data.fd = ClientFd;

                //将新的ClientFd添加到epoll的监听队列中
                if (epoll_ctl(gnEpollSocket, EPOLL_CTL_ADD, ClientFd, &event) < 0)
                {
                    log_info("添加socket '%d' 到 epoll 监听队列失败: %s\n",
                                    ClientFd, strerror(errno));
                    return NULL;
                }

                in_add_socket(ClientFd,
                              Client_Addr.sin_addr.s_addr,
                              ntohs(Client_Addr.sin_port));
                continue;
            }
            //获取错误事件!
            else if((pEvents[i].events & EPOLLERR) || (pEvents[i].events & EPOLLHUP))
            {
                struct sockaddr_in Client_Addr;
                socklen_t socklen = sizeof(struct sockaddr_in);


                // log_info("EPOLLERR | EPOLLHUP:pEvents[%d].data.fd=%d,pEvents[%d].events=%d\n",
                //                 i,pEvents[i].data.fd,i,pEvents[i].events);

                memset(&Client_Addr, 0, sizeof(socklen));
                getpeername(pEvents[i].data.fd,
                            (struct sockaddr *)&Client_Addr,
                            &socklen);

                // log_info("接入端出现问题,即将关闭连接\n");

                in_close_socket(pEvents[i].data.fd);

                continue;
            }
            else
            {
                if (read_buffer(pEvents[i].data.fd)<0)
                {
                    epoll_ctl(gnEpollSocket,
                              EPOLL_CTL_DEL,
                              pEvents[i].data.fd,&event);
                }
            }
        }
    }
    if(gpSocketArray)
    {
        free(gpSocketArray);
    }
    if (pEvents)
    {
        free(pEvents);
    }
    if (buf)
    {
        free(buf);
        buf=NULL;
    }
    

    close(gnListenSocket);
    close(gnEpollSocket);

    return NULL;
}



// 设置SOCKET为非阻塞模式
int in_make_socket_non_blocking (int Socket_Fd)
{
    int flags;
    flags = fcntl (Socket_Fd, F_GETFL, 0);
    if (flags == -1)
    {
        log_info("获取SOCKET的标志错误[%s]\n", strerror(errno));
        return -1;
    }

    flags |= O_NONBLOCK;

    if (fcntl (Socket_Fd, F_SETFL, flags) == -1)
    {
        log_info("设置SOCKET的标志错误[%s]\n", strerror(errno));
        return -1;
    }
    return 0;
}

//设置socket,且连接保活/
int set_Socket (int Socket_Fd,socklen_t socklen,int Cache)
{
    //断网响应,错误的时间,单位(秒)
    // 如该连接在20秒内没有任何数据往来,则进行探测;
    int keepIdle     = NO_RESPONSE_TIME;
    // 探测时发包的时间间隔为8秒
    int keepInterval = DETECTING_PACKET_TIME_INTERVAL;
    // 探测尝试的次数.若第1次探测包就收到响应,则不再发.
    int keepCount    = DETECTION_COUNT;

    setsockopt(Socket_Fd ,
               SOL_SOCKET ,
               SO_KEEPALIVE ,
               (const char*)&socklen ,
               sizeof(socklen) );

    setsockopt(Socket_Fd,
               SOL_TCP,
               TCP_KEEPIDLE,
               (void *)&keepIdle,
               sizeof(keepIdle));

    setsockopt(Socket_Fd,
               SOL_TCP,
               TCP_KEEPINTVL,
               (void *)&keepInterval,
               sizeof(keepInterval));

    setsockopt(Socket_Fd,
               SOL_TCP,
               TCP_KEEPCNT,
               (void *)&keepCount,
               sizeof(keepCount));

    //设置TCP_NODELAY
    setsockopt(Socket_Fd,
               IPPROTO_TCP,
               TCP_NODELAY,
               &Cache, 
               sizeof(Cache));

    in_make_socket_non_blocking(Socket_Fd);
    return 0;
}


//创建socket
int epoll_Socket_Start()
{
    int i = 1;
    struct sockaddr_in Server_Addr;

    bzero(&Server_Addr, sizeof(Server_Addr));
    
    Server_Addr.sin_addr.s_addr = INADDR_ANY;

    Server_Addr.sin_family = AF_INET;
    Server_Addr.sin_port = htons (LISTEN_PORT);

    //创建socket
    if((gnListenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_info("创建 AF_INET socket 错误:%s\n", strerror(errno));
        return -1;
    }
    //设置监听套接字
    if(setsockopt(gnListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i))<0)
    {
        log_info("设置监听 socket SO_REUSEADDR 错误:%s\n", strerror(errno));
        close(gnListenSocket);
        return -1;
    }
    // 设置SOCKET为非阻塞模式
    if (in_make_socket_non_blocking(gnListenSocket) < 0)
    {
        log_info("设置SOCKET为非阻塞模式错误:%s\n", strerror(errno));
    }


    //设置socket重用
    if(setsockopt(gnListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(gnListenSocket);
        return -1;
    }

    //绑定端口
    if (bind(gnListenSocket, (struct sockaddr *) &Server_Addr, sizeof(struct sockaddr)) == -1)
    {
        log_info(" bind address %d:%d error:%s\n",
                        INADDR_ANY,
                        LISTEN_PORT,
                        strerror(errno));
        close(gnListenSocket);
        return -1;
    }
    //监听网络
    if (listen(gnListenSocket, LISTEN_QUEUE_SIZE) == -1)
    {
        log_info("监听网络错误:%s\n", strerror(errno));
        return -1;
    }
    /* 创建 epoll 句柄,把监听 socket 加入到 epoll 集合里 */
    if((gnEpollSocket = epoll_create(EPOLL_MAX_EVENTS)) < 0)
    {
        log_info("epoll_create socket error:%s\n", strerror(errno));
        close(gnListenSocket);
        return -1;
    }

    log_info("start,端口号:%d,最大连接数:%d,等待连接队列的最大长度:%d\n",
                    LISTEN_PORT,
                    EPOLL_MAX_EVENTS,
                    LISTEN_QUEUE_SIZE);


    struct epoll_event event;
    
    // event.events = EPOLLIN | EPOLLET;//设置epoll ET模式
    event.events = EPOLL_MOAD;//设置epoll LT模式
    event.data.fd = gnListenSocket;

    if (epoll_ctl(gnEpollSocket, EPOLL_CTL_ADD, gnListenSocket, &event) < 0)
    {
        log_info("add  epoll socket error:%s\n", strerror(errno));
        close(gnListenSocket);
        close(gnEpollSocket);
        return -1;
    }
    if(pthread_create(&gnMainThread, NULL, epoll_main_thread_start, NULL) < 0)
    {
        log_info("创建网络接收主线程失败:%s\n", strerror(errno));
        close(gnListenSocket);
        close(gnEpollSocket);
        return -1;
    }
    else
    {
        log_info("创建网络接收主线程成功\n");

        while (1)
        {
           log_info("当前连接数:[%d]\n",acceptCount);
           sleep(10);
        }
        

    }

    return 0;
}



int read_buffer(int connfd)
{
    int i,read_size=0;
    SocketInfo *pClientInfo = NULL;
    struct sockaddr_in Client_Addr;
    socklen_t in_len = sizeof(Client_Addr);
    memset(&Client_Addr, 0, sizeof(Client_Addr));
    getpeername(connfd, (struct sockaddr *)&Client_Addr, &in_len);

    pthread_mutex_lock(&gstruMutex);
    for(i = 0; i< gnSocketArrayNum; i ++)
    {
        if(gpSocketArray[i] && gpSocketArray[i]->socket == connfd)
        {
            pClientInfo = gpSocketArray[i];
            break;
        }
    }
    pthread_mutex_unlock(&gstruMutex);

    if(pClientInfo == NULL)
    {
        log_info("无法找到对应的内部句柄,内部错误？\n");
        return -1;
    }

    // 处理客户端请求,读取客户端socket流
    TaskInfo *task = (TaskInfo*)malloc(sizeof(TaskInfo));
    task->socket = pClientInfo->socket;
    task->pszBuffer = (char*)malloc(MAXLINE);
    memset(task->pszBuffer,0,MAXLINE);

    read_size = recv(pClientInfo->socket, task->pszBuffer, MAXLINE,0);
    if(read_size > 0)
    {

        log_info("recv %s\n",task->pszBuffer);
        // 调用外部函数进行报文处理
        if(gCallBackFunc)
            gCallBackFunc(task);
    }
    else if(read_size == 0)
    {
        // log_info("客端主动关闭连接[%s:%d]\n",
        //                 inet_ntoa(Client_Addr.sin_addr),
        //                 pClientInfo->nPort);
        in_close_socket(pClientInfo->socket);
        return -1;
    }

    return 0;
}

// 新接收到一个连接
void in_add_socket(int nFd, unsigned long nIp, unsigned short nPort)
{
    int i;
    SocketInfo *pClientInfo = NULL;
    pClientInfo = (SocketInfo *)malloc(sizeof(SocketInfo));
    memset(pClientInfo, 0, sizeof(SocketInfo));

    struct in_addr addr = {nIp};
    log_info("当前接入客户端IP[%s],Port[%d]",inet_ntoa(addr),nPort);

    pClientInfo->socket = nFd;
    pClientInfo->nIP    = nIp;
    pClientInfo->nPort  = nPort;


    pthread_mutex_lock(&gstruMutex);
    //填在原有数组的空位上(释放后留出来的空位)
    for(i = 0; i< gnSocketArrayNum; i ++)
    {
        if(gpSocketArray[i] == NULL)
        {
            gpSocketArray[i] = pClientInfo;
            break;
        }
    }
    if(i == gnSocketArrayNum)
    {
        log_info("resize socket cache from %d to %d\n",
                        gnSocketArrayNum,
                        gnSocketArrayNum + MIN_SOCKET_Array_NUM);

        gnSocketArrayNum += MIN_SOCKET_Array_NUM;
        gpSocketArray = (SocketInfo **)realloc(gpSocketArray, gnSocketArrayNum * sizeof(SocketInfo *));

        for( i = gnSocketArrayNum - MIN_SOCKET_Array_NUM; i < gnSocketArrayNum; i ++)
            gpSocketArray[i] = NULL;

        gpSocketArray[gnSocketArrayNum - MIN_SOCKET_Array_NUM] = pClientInfo;
    }
    pthread_mutex_unlock(&gstruMutex);

    acceptCount++;
}

// 关闭一个连接
void in_close_socket(int nFd)
{
    int i;
    SocketInfo *pClientInfo = NULL;

    struct sockaddr_in Client_Addr;
    socklen_t in_len = sizeof(Client_Addr);
    memset(&Client_Addr, 0, sizeof(Client_Addr));
    getpeername(nFd, (struct sockaddr *)&Client_Addr, &in_len);

    pthread_mutex_lock(&gstruMutex);
    for(i = 0; i< gnSocketArrayNum; i ++)
    {
        if(gpSocketArray[i] && gpSocketArray[i]->socket == nFd)
        {
            pClientInfo = gpSocketArray[i];
            gpSocketArray[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&gstruMutex);

    if(pClientInfo)
    {
        log_info("关闭一个连接 \n");
        free(pClientInfo);
        pClientInfo =NULL;
        epoll_ctl(gnEpollSocket, EPOLL_CTL_DEL, nFd, NULL);
        close(nFd);
        acceptCount--;
    }
    else
    {
        log_info("没有找到对应的句柄\n");
    }
}
