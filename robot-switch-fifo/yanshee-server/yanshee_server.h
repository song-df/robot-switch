//
//  DataTransfer.h
//  EpollServer
//
//  Created by zkz on 20/8/04.
//  Copyright ? 2020年 zkz. All rights reserved.
//

#ifndef yanshee_wrap_server_h
#define yanshee_wrap_server_h


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

#include "../liblog/liblog.h"
#include "task_work.h"
#include "../libtaskList/libtaskList.h"
#include "epoll_socket.h"


#endif /* yanshee_server_h */
