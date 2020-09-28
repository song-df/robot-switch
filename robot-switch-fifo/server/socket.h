#ifndef _SOCKET_H
#define _SOCKET_H
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
#include <sys/socket.h>      /* basic socket definitions */
#include "../liblog/liblog.h"

#define LISTEN_PORT                      33780  //客户端端口
#define LISTEN_QUEUE_SIZE                1024   //等待连接队列的最大长度
int Create_Socket(int * ListenSocket);  

#endif /* _SOCKET_H */