#ifndef _INIT_DAEMON_H
#define _INIT_DAEMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <pthread.h>

int init_daemon(void);
#endif /* _INIT_DAEMON_H */