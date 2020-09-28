#include "DoWork.h"

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
			send(clientFD, buff, strlen(buff), MSG_NOSIGNAL);
		}
    }
end:
	if (buff)
	{
		free(buff);
	}	
    return NULL;
}