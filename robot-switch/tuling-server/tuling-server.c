#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>  
#include "tuling-server.h"
#include "init_daemon.h"
#include "../liblog/liblog.h"
#include "../libtulingApi/libtulingApi.h"
extern int errno;

int init_daemon(void) 
{ 
	int pid; 
	int i; 
	//忽略终端I/O信号，STOP信号
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	
	pid = fork();
	if(pid > 0) 
	{
		exit(0); //结束父进程，使得子进程成为后台进程
	}
	else if(pid < 0) 
	{ 
		return -1;
	}
	//建立一个新的进程组,在这个新的进程组中,子进程成为这个进程组的首进程,以使该进程脱离所有终端
	setsid();
	//再次新建一个子进程，退出父进程，保证该进程不是进程组长，同时让该进程无法再打开一个新的终端
	pid=fork();
	if( pid > 0) 
	{
		exit(0);
	}
	else if( pid< 0) 
	{
		return -1;
	}
	//关闭所有从父进程继承的不再需要的文件描述符
	for(i=0;i< NOFILE;close(i++));
	//改变工作目录，使得进程不与任何文件系统联系
	i = chdir("/");
	//将文件当时创建屏蔽字设置为0
	umask(0);
	//忽略SIGCHLD信号
	signal(SIGCHLD,SIG_IGN); 
	return 0;
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
void* DoWork(void *arg)
{
	int read_size = 0;
	int clientFD = *(int*)arg;
	char * buff = (char*)malloc(1024);

	struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    socklen_t len = sizeof( timeout );
    //设置接收超时
    if(setsockopt(clientFD,SOL_SOCKET,SO_RCVTIMEO,&timeout,len) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(clientFD);
        goto end;
    }
	
	InitTulingApi();

    while (1)
    {
		if(!CheckSocketConnected(clientFD))
        {
            log_info("Client not online goto end\n");
			break;
        }
		read_size = recv(clientFD, buff, 1024,MSG_WAITALL);
    	if(read_size > 0)
    	{
			log_info("recv %s\n",buff);

			CallTulingApi(clientFD ,TULING_APIKEY, 
				   		  buff, 1024,
				   		  buff, 1024);
			send(clientFD, buff, strlen(buff), MSG_NOSIGNAL);
		}
    }
end:
	ReleaseTulingApi();
	if (buff)
	{
		free(buff);
	}	
    return NULL;
}



int main(int argc, char **argv)
{
    log_init(LOG_RSYSLOG, NULL);
    init_daemon();

    int ListenSocket;
	int i = 1;
    struct sockaddr_in Server_Addr;
    bzero(&Server_Addr, sizeof(Server_Addr));
    Server_Addr.sin_addr.s_addr = INADDR_ANY;
    Server_Addr.sin_family = AF_INET;
    Server_Addr.sin_port = htons (LISTEN_PORT);

    //创建socket
    if((ListenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_info("创建 AF_INET socket 错误:%s\n", strerror(errno));
        return -1;
    }
    //设置监听套接字
    if(setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i))<0)
    {
        log_info("设置监听 socket SO_REUSEADDR 错误:%s\n", strerror(errno));
        close(ListenSocket);
        return -1;
    }

    //设置socket重用
    if(setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(ListenSocket);
        return -1;
    }

    //绑定端口
    if (bind(ListenSocket, (struct sockaddr *) &Server_Addr, sizeof(struct sockaddr)) == -1)
    {
        log_info(" bind address %d:%d error:%s\n",
                        INADDR_ANY,
                        LISTEN_PORT,
                        strerror(errno));
        close(ListenSocket);
        return -1;
    }
    //监听网络
    if (listen(ListenSocket, LISTEN_QUEUE_SIZE) == -1)
    {
        log_info("监听网络错误:%s\n", strerror(errno));
        return -1;
    }


	int sin_size;
	int clientFD;
	struct sockaddr_in client_addr;
	while(1)
	{

		sin_size = sizeof(struct sockaddr);
		clientFD = accept(ListenSocket,(struct sockaddr*)(&client_addr),&sin_size);
		log_info("server get connection from %s\n",inet_ntoa(client_addr.sin_addr));
		
		pthread_t thread;
    	pthread_attr_t attr;
		// 创建服务线程
		memset(&attr, 0, sizeof(attr));
		//设置线程分离属性
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &attr, DoWork, &clientFD);
	}

    log_deinit();
    return 0;
}
