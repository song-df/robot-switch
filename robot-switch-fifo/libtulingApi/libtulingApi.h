
#ifndef __TULING_API_H__
#define __TULING_API_H__


#ifdef __cplusplus
extern "C" {
#endif


int  InitTulingApi();
int  CallTulingApi(int userID ,const char * apiKey, 
				   const char * text,int textLen,
				   char * buff,int buffLen);
int  ReleaseTulingApi();

#ifdef __cplusplus
}
#endif
#endif
