
#ifndef _SERVER_H
#define _SERVER_H
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
#define LISTEN_PORT                      33780  //客户端端口
#define LISTEN_QUEUE_SIZE                1024   //等待连接队列的最大长度


#endif /* _SERVER_H */
