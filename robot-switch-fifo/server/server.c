
#include "server.h"

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

int main(int argc, char **argv)
{
    log_init(LOG_RSYSLOG, NULL);
    init_daemon();

    start();

    log_deinit();
    return 0;
}

void start()
{
	int ListenSocket;
	Create_Socket(&ListenSocket);
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
	return;
}





