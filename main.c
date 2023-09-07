#include "stdio.h"
#include "noman.h"
#include <windows.h>

//char nomanImeiTest[] = {"s0.0.0.0c2.2.0.3.11111111111,222222222222222,33333333333,44444444444,55555555555,66666666666,77777777777,88888888888,99999999999"};
//char hostImeiTest[] = {"123456789012345,123456789012346"};


char nomanImeiTest[2048];
char hostImeiTest[500];

int main()
{
	int ret = 0;
	
	noman_init(&MyNomanAPI, &MyNomanCB);
	printf("����������IMEI:��ʽΪ imei,imei,imei\r\n");
	scanf("%s",hostImeiTest);
	printf("����������ֵ��ָ��:��ʽΪ s0.0.0.0c2.2.0.3.11111111111,222222222222222,33333333333 ....\r\n");
	scanf("%s",nomanImeiTest);
	/*��ȡ����IMEI*/
	ret = host_imei_parse(&MyNomanCB,hostImeiTest);
//	
	/*��ӡ������Ϣ*/
	printf("Host IMEI num: %d\r\n",ret);
	for(int i; i<3; i++){
		printf("Host IMEI%d: %s\r\n",i+1,MyNomanCB.host[i].imei);
	}
//	
	/*��ȡ����ֵ��IMEI*/ 
	ret = noman_status_parse(&MyNomanCB,nomanImeiTest);
	ret = noman_imei_parse(&MyNomanCB,nomanImeiTest+8);
	
	int temp;
	while(1){
		printf("[%4d]\r\n",temp++);
		ret = noman_televise(&MyNomanCB, 1);
		if(ret == 3){
			/*�˴��ɼ��벦��Ӧ�����ĺ���*/
			break;
		}
		Sleep(100);
	}
}



