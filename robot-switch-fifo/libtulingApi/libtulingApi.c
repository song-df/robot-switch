
#include "libtulingApi.h"
#include "md5.h"
#include "libjson.h"
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

static cJSON * root  = NULL;
static cJSON * item  = NULL;   
static cJSON * item2 = NULL;  
#define POSTURL    "http://openapi.tuling123.com/openapi/api/v2"

int  InitTulingApi()
{
	char * jsonStr = "                                          \
    {                                                           \
    \"reqType\":0,                                              \
    \"perception\":                                             \
        {                                                       \
            \"inputText\":                                      \
            {                                                   \
                \"text\": \"附近的酒店\"                          \
            },                                                  \
            \"selfInfo\":                                       \
            {                                                   \
                \"location\":                                   \
                {                                               \
                    \"city\": \"北京\",                          \
                    \"province\": \"北京\",                      \
                    \"street\": \"长安街\"                       \
                }                                               \
            }                                                   \
        },                                                      \
    \"userInfo\":                                               \
        {                                                       \
            \"apiKey\": \"018bd979f8dd4a90866587ae6aa040e5\",   \
            \"userId\": \"018bd979f8dd4a90866587ae6aa040e5\"    \
        }                                                       \
    }";
	root  = cJSON_Parse(jsonStr);  
    if (!root) 
    {
		return -1;
    }
    else
    {
        item = cJSON_GetObjectItem(root, "perception");
        item =cJSON_GetObjectItem(item,"inputText");

        item2 = cJSON_GetObjectItem(root, "userInfo");
    }

	return 0;
}
int ReleaseTulingApi()
{
	if (root)
	{
		cJSON_Delete(root);
	}

	return 0;
}


size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream)
{
    char *fptr = (char *)stream;
    memcpy(fptr,buffer,size*nmemb);
    return size*nmemb;
}


int CallTulingApi(int userID ,const char * apiKey, 
				  const char * text,int textLen,
				  char * buff,int buffLen)
{
	if (!root)
	{
		return -1;
	}
	
	char encrypt[33]={0};
	snprintf(encrypt,32,"%d",userID);
	unsigned char decrypt[16];   
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, (unsigned char *)encrypt, strlen((char *)encrypt));
	MD5Final(&md5, decrypt);
	char userId[33] ={ 0 };
	for (int i=0;i<16;i++)
	{
		snprintf(userId+2*i, 32, "%02x", decrypt[i]);
	}
	cJSON_ReplaceItemInObject(item, "text", cJSON_CreateString(text));
	cJSON_ReplaceItemInObject(item2, "userId", cJSON_CreateString(userId));
	cJSON_ReplaceItemInObject(item2, "apiKey", cJSON_CreateString(apiKey));
	char * POSTFIELDS = cJSON_Print(root);

	CURL *curl;

	char *fptr = (char *)malloc(buffLen+1024);
	curl = curl_easy_init();
	if (!curl)
	{
		fprintf(stderr, "curl init failed\n");
		return -1;
	}
	curl_easy_setopt(curl, CURLOPT_URL, POSTURL);              //url地址
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, POSTFIELDS);    //post参数
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //对返回的数据进行操作的函数地址
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);           //这是write_data的第四个参数值
	curl_easy_setopt(curl, CURLOPT_POST, 1);                   //设置问非0表示本次操作为post
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);                //打印调试信息
	curl_easy_setopt(curl, CURLOPT_HEADER, 1);                 //将响应头信息和相应体一起传给write_data
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);         //设置为非0,响应头信息location
	curl_easy_perform(curl);
	
	if(POSTFIELDS)
	{
		free(POSTFIELDS);
	}
	cJSON * RecvRoot= cJSON_Parse(fptr); 
	if(fptr)
	{
		free(fptr);
	}
	cJSON * RecvItem  = cJSON_GetObjectItem(RecvRoot, "results");
	int array_size = cJSON_GetArraySize(RecvItem);
	for (int iCnt = 0; iCnt < array_size; iCnt++)
    {
		cJSON *pSub = cJSON_GetArrayItem(RecvItem, iCnt);
		if (NULL == pSub)
		{
			continue;
		}
		cJSON * pResultType = cJSON_GetObjectItem(pSub,"resultType");
		char * str_ResultType = cJSON_Print(pResultType);
		if (strcmp(str_ResultType,"\"text\"") == 0)//是
		{
			char * pvalues =NULL;
			cJSON * pIvalues = cJSON_GetObjectItem(pSub,"values");
			pIvalues = cJSON_GetObjectItem(pIvalues,"text");
			pvalues = cJSON_Print(pIvalues);
			int len = strlen(pvalues);
			len = len >=buffLen ? buffLen:len;
			memset(buff,0,buffLen);
			memcpy(buff,pvalues+1,len-2);
			if(pvalues)
			{
				free(pvalues);
			}
			break;
		}
		if (str_ResultType)
		{
			free(str_ResultType);
		}
	}
	cJSON_Delete(RecvRoot);
	curl_easy_cleanup(curl);

	return 0;
	
}
