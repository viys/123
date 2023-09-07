#include "stdio.h"
#include "noman.h"
#include <windows.h>

char nomanImeiTest[] = {"AABBs0.0.0.0c2.2.0.3.11111111111,222222222222222,33333333333,44444444444,55555555555,66666666666,77777777777,88888888888,99999999999"};
char hostImeiTest[] = {"123456789012345,123456789012346"};

extern char aNI[NOMAN_NUM][15];

int main()
{
	int ret = 0;
	
	noman_init(&MyNomanAPI, &MyNomanCB);
	
	/*获取主机IMEI*/
	ret = host_imei_parse(&MyNomanCB,hostImeiTest);
//	
	/*打印调试信息*/
	printf("aNI mem size:%d\r\n",sizeof(aNI));
	printf("Host IMEI num: %d\r\n",ret);
	for(int i; i<3; i++){
		printf("Host IMEI%d: %s\r\n",i+1,MyNomanCB.host[i].imei);
	}
//	
//	/*获取无人值守IMEI*/ 
	ret = noman_status_parse(&MyNomanCB,nomanImeiTest+4);
	ret = noman_imei_parse(&MyNomanCB,nomanImeiTest+12);
	
	int temp;
	while(1){
		printf("[%4d]\r\n",temp++);
		ret = noman_televise(&MyNomanCB, 1);
		printf("ret %d\r\n",ret);
		if(ret == 3){
			break;
		}
		Sleep(100);
	}
}



