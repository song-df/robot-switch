#ifndef __EPOLL_SOCKET_H__
#define __EPOLL_SOCKET_H__

#include "echo-server.h"
#include <sys/socket.h>      /* basic socket definitions */
#include <sys/epoll.h>       /* epoll function */

//#define EPOLL_MOAD (EPOLLIN)                  //epoll LT模式 水平触发
#define EPOLL_MOAD (EPOLLIN | EPOLLET)          //epoll ET模式 边缘触发
#define MAXLINE                          (1024*4)   //socket数据包大小
#define SOCKET_CACHE_SIZE                (1024) //socket缓冲区大小
#define MIN_SOCKET_Array_NUM             10
#define EPOLL_MAX_EVENTS                 100000 //Epoll 最大事件数

#define LISTEN_QUEUE_SIZE                1024   //等待连接队列的最大长度
#define LISTEN_PORT                      33780  //客户端端口

//单位是秒
#ifndef TIME_OUT
#define TIME_OUT
#define NO_RESPONSE_TIME                  4     //没响应时间
#define DETECTING_PACKET_TIME_INTERVAL    3     //探测包时间间隔
#define DETECTION_COUNT                   2     //探测次数
#endif

typedef struct _SocketInfo {
    int             socket;				// 客户端的SOCKET
    unsigned long   nIP;				// 客户端的IP
    unsigned short  nPort;				// 客户端的端口
}SocketInfo;

typedef struct _TaskInfo {
	int             socket;				// 客户端的SOCKET
	unsigned char	*pszBuffer;			// 缓存的数据
}TaskInfo;

static int acceptCount =0;                        // 链接数量
static int gnListenSocket;					// 监听套接字
static int gnEpollSocket;					// EPOLL套接字
static pthread_t gnMainThread;				// 主线程
static int gnSocketArrayNum = 0;
static SocketInfo **gpSocketArray = NULL;
static pthread_mutex_t gstruMutex = PTHREAD_MUTEX_INITIALIZER;

typedef int (*OnRecvPackageCallback)(TaskInfo *buf);

int epoll_Socket_Start();                                       //创建Socket


int set_Socket (int Socket_Fd,socklen_t socklen,int Cache);  //设置socket
int in_make_socket_non_blocking (int Socket_Fd);             // 设置SOCKET为非阻塞模式


void in_add_socket(int nFd,unsigned long nIp,unsigned short nPort); //将链接添加到epoll
int read_buffer(int connfd);                                        // 读取socket
void in_close_socket(int nFd);                                      // 关闭一个连接

int epoll_socket_set_callback(OnRecvPackageCallback callback);


#endif /* epoll_socket */
