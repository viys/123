#include "stdio.h"
#include "noman.h"
#include <windows.h>

char nomanImeiTest[] = {"AABB01s0.1.0.1.11111111111,222222222222222,33333333333,44444444444,55555555555,66666666666,77777777777,88888888888,99999999999"};
char hostImeiTest[] = {"123456789012345,123456789012346,123456789012347"};

extern char aNI[NOMAN_NUM][15];

int main()
{
	int ret = 0;
	/*获取主机IMEI*/
	ret = host_imei_parse(hostImeiTest);
	
	/*打印调试信息*/
	printf("aNI mem size:%d\r\n",sizeof(aNI));
	printf("Host IMEI num: %d\r\n",ret);
	for(int i; i<3; i++){
		printf("Host IMEI%d: %s\r\n",i+1,MyNomanCB.host[i].imei);
	}
	
	/*获取无人值守IMEI*/ 
	ret = noman_imei_parse(nomanImeiTest+6);
	int temp = 1;
	while(1){
		ret = noman_televise(MyNomanCB);		
		if(ret==2)break;
		
		printf("[%d] 电梯正在呼叫...\r\n",temp++);
		
		Sleep(1000);
	}
}



