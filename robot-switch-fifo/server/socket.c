#include "socket.h"


extern int errno;

int Create_Socket(int* ListenSocket)
{
	int i = 1;
    struct sockaddr_in Server_Addr;

    bzero(&Server_Addr, sizeof(Server_Addr));
    Server_Addr.sin_addr.s_addr = INADDR_ANY;
    Server_Addr.sin_family = AF_INET;
    Server_Addr.sin_port = htons (LISTEN_PORT);

    //创建socket
    if((*ListenSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        log_info("创建 AF_INET socket 错误:%s\n", strerror(errno));
        return -1;
    }
    //设置监听套接字
    if(setsockopt(*ListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i))<0)
    {
        log_info("设置监听 socket SO_REUSEADDR 错误:%s\n", strerror(errno));
        close(*ListenSocket);
        return -1;
    }

    //设置socket重用
    if(setsockopt(*ListenSocket, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
    {
        log_info("设置SOCKET为SO_REUSEADDR模式错误:%s\n",strerror(errno));
        close(*ListenSocket);
        return -1;
    }

    //绑定端口
    if (bind(*ListenSocket, (struct sockaddr *) &Server_Addr, sizeof(struct sockaddr)) == -1)
    {
        log_info(" bind address %d:%d error:%s\n",
                        INADDR_ANY,
                        LISTEN_PORT,
                        strerror(errno));
        close(*ListenSocket);
        return -1;
    }
    //监听网络
    if (listen(*ListenSocket, LISTEN_QUEUE_SIZE) == -1)
    {
        log_info("监听网络错误:%s\n", strerror(errno));
        return -1;
    }

	return 0 ;
}

