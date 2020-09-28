#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h> 
#include "include/RobotApi.h"


int  disConnectRobot()
{
	ubtRobotDisconnect("SDK","1","127.0.0.1");
	ubtRobotDeinitialize();
	return 0;
}

int  connectRobot()
{
	ubtRobotInitialize();
	if (ubtRobotConnect("SDK","1","127.0.0.1"))
	{
		ubtVoiceTTS(0,"无法连接Yanshee");
		return -1;
	}
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

int main(int argc, char const *argv[])
{
	if (connectRobot() !=0 )
	{
		printf("机器人挂啦\n");
		return -1;
	}
	int tcp_socket_fd = socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server_addr = {0};//要连接服务器的地址
    server_addr.sin_family = AF_INET;//使用IPv4
	server_addr.sin_port = htons(18733);//指定要连接服务器的某个端口， 端口是int类型，范围0~65535
	server_addr.sin_addr.s_addr = inet_addr("124.70.148.79");//服务器的ip， ip是字符串类型，要进行相应的转换
	
	connect(tcp_socket_fd ,(struct sockaddr *)&server_addr,sizeof(server_addr));//发起连接服务器请求
	char * buff = (char *)malloc(1024);
	memset(buff,0,1024);


	struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    socklen_t len = sizeof( timeout );
    //设置接收超时
    if(setsockopt(tcp_socket_fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,len) < 0)
    {
        close(tcp_socket_fd);
        return 0;
    }

	int read_size = 0;
	while (1)
	{
		if(!CheckSocketConnected(tcp_socket_fd))
        {
			break;
        }
		read_size = recv(tcp_socket_fd, buff, 1024,MSG_WAITALL);
		if(read_size > 0)
    	{
			send(tcp_socket_fd, "start", 5, MSG_NOSIGNAL);
			ubtStartRobotAction(buff,1);
			send(tcp_socket_fd, "done", 4, MSG_NOSIGNAL);

		}
		memset(buff,0,1024);
	}

	disConnectRobot();

	return 0;
}

