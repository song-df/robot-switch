#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "echo-server.h"
#include "init_daemon.h"
#include "../liblog/liblog.h"
extern int errno;



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
			send(clientFD, buff, strlen(buff), 0);
		}
    }
end:
	if (buff)
	{
		free(buff);
	}	
    return NULL;
}



int main(int argc, char **argv)
{
    log_init(LOG_RSYSLOG, NULL);

	//把当前程序设置为后台守护进程
    init_daemon(); 

    int ListenSocket;
	int i = 1;


    //创建socket
    if((ListenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_info("创建 AF_INET socket 错误:%s\n", strerror(errno));
        return -1;
    }

    //设置socket重用 
	//socket默认情况下 在程序退出(包括异常退出) 系统不会自动释放socket 大约需要1分钟后会被系统释放
	// 设置SO_REUSEADDR 属性，可以直接重复使用该端口
    if(setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(ListenSocket);
        return -1;
    }
	
	struct sockaddr_in Server_Addr;           //创建Socket address 
    bzero(&Server_Addr, sizeof(Server_Addr));  //memset() 0 
    Server_Addr.sin_addr.s_addr = INADDR_ANY;  //任意地址 ，也可以是指定地址，服务器为了接受任何IP链接来的，要设置为INADDR_ANY
    Server_Addr.sin_family = AF_INET;          // IPv4 进行通信
    Server_Addr.sin_port = htons (LISTEN_PORT); //监听端口号

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
		
		
    	pthread_attr_t attr;
		memset(&attr, 0, sizeof(attr));
		//设置线程分离属性
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_t thread;
		pthread_create(&thread, &attr, DoWork, &clientFD);
	}

    log_deinit();
    return 0;
}
