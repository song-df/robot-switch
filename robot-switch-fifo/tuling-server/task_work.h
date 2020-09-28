#ifndef __TASK_WORKER_H__
#define __TASK_WORKER_H__


#include "tuling_server.h"
#include "../libtaskList/libtaskList.h"

#define POSTURL    "http://openapi.tuling123.com/openapi/api/v2"
#define TULING_APIKEY	"9a1cd980f8e8463f87b42536ba58caab"

// 启动函数
int task_worker_start();

#endif
