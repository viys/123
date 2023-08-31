/**
 * @file noman.c
 * @author viys (viys@yu.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "noman.h"
#include "string.h"

/*���е�����ֵ��IMEI�����*/
char aNI[NOMAN_NUM][15];

/*����ֵ�ؿ��ƿ�*/
/*���ļ�Ϊ�ֻ�����ֵ��*/
Noman_CB MyNomanCB;

uint8 noman_televise(Noman_CB Noman)
{
	uint8 ret = 0;
	if(MyNomanCB.host[3].nomanNum != 0){
		/*����ֻ���������ֵ��*/ 
		ret = call_noman(&MyNomanCB,3);
		if(ret==2){
			/*�ֺ����*/
			printf("�ֺ����\r\n");
		}
	}else{
		ret = call_noman(&MyNomanCB,MyNomanCB.hostStatus);
		if(ret==2){
			/*�ֺ����*/
			MyNomanCB.hostStatus = 0;
			printf("�ֺ����\r\n");
		}else if(ret==1){
			MyNomanCB.hostStatus++;
		}
	}
	return ret;
}

int noman_imei_parse(char *data)
{
    uint8 ret = 0;
    memset(aNI,0x00,sizeof(aNI));
    /*��������������Ӧ������ֵ�غ�����*/
    ret = sscanf(data,"s%d.%d.%d.%d.",
                &MyNomanCB.host[0].nomanNum, &MyNomanCB.host[1].nomanNum, &MyNomanCB.host[2].nomanNum, &MyNomanCB.host[3].nomanNum);
    data += 9;

    /*��������ֵ���ܺ�����*/
    MyNomanCB.allNum = MyNomanCB.host[0].nomanNum + MyNomanCB.host[1].nomanNum + MyNomanCB.host[2].nomanNum + MyNomanCB.host[3].nomanNum;
    
    printf("Noman IMEI num: %d\r\n",MyNomanCB.allNum);
    printf("HostI num:%d,HostII num:%d,HostIII num:%d,Slave num:%d\r\n",MyNomanCB.host[0].nomanNum,MyNomanCB.host[1].nomanNum,MyNomanCB.host[2].nomanNum,MyNomanCB.host[3].nomanNum);
    
	/*��������ֵ��IMEI*/
    for(uint8 i=0; i<MyNomanCB.allNum; i++){
        ret = sscanf(data,"%[0-9],",aNI[i]);
        /**/
        data += strlen(aNI[i]) + 1;
        if(strlen(aNI[i])==11){
        	memcpy(&aNI[i][11],"0000",4);
		}
		
        printf("Noman ID: %d -> %s >> size:%d\r\n",i+1,aNI[i],strlen(aNI[i]));
    }

    /*��������ֵ�ص�ַ*/
    MyNomanCB.hostStatus = 0;
    MyNomanCB.Start = aNI[0];
    MyNomanCB.Out = MyNomanCB.Start;
    MyNomanCB.End = aNI[MyNomanCB.allNum-MyNomanCB.host[3].nomanNum-1];
    MyNomanCB.host[0].ptrStart = MyNomanCB.Start;
    MyNomanCB.host[0].ptrOut = MyNomanCB.host[0].ptrStart;
    MyNomanCB.host[0].ptrEnd = MyNomanCB.host[0].ptrStart + MyNomanCB.host[0].nomanNum*15;
    for(uint8 i=1; i<4; i++){
        MyNomanCB.host[i].ptrStart = MyNomanCB.host[i-1].ptrEnd;
        MyNomanCB.host[i].ptrOut = MyNomanCB.host[i].ptrStart;
        MyNomanCB.host[i].ptrEnd = MyNomanCB.host[i].ptrStart + MyNomanCB.host[i].nomanNum*15;
    }
    return MyNomanCB.allNum;
}

uint8 host_imei_parse(char *data)
{
    MyNomanCB.hostNum = sscanf(data,"%[0-9],%[0-9],%[0-9],",MyNomanCB.host[0].imei,MyNomanCB.host[1].imei,MyNomanCB.host[2].imei);
    return MyNomanCB.hostNum;
}

/*�ڶ��ν����жϵ�ʱ��Ҫֱ�Ӳ�����һ��*/
uint8 call_noman(Noman_CB* Noman, uint8 hostID)
{
    uint8 ret = 0;
    
    if((Noman->Out == Noman->End && Noman->hostStatus == Noman->hostNum) || (Noman->host[3].nomanNum != 0 && Noman->Out == Noman->host[3].ptrEnd-15) || Noman->Out == (char*)&(Noman->host[Noman->hostNum - 1].imei)){
			/*��������������ֵ�ػ�༶���е����һ��*/
			cancel_imei(Noman->Out);
			MyNomanCB.Out = MyNomanCB.Start;
			return 2;  
	}
	/*��������´��ֺ���Ч�����������һ��*/
//	if(Noman->host[hostID].ptrOut == Noman->host[hostID].ptrEnd && (Noman->hostStatus < Noman->hostNum)){
//    	Noman->hostStatus++;
//		hostID++;
//	}
	/*�����߼�*/
	if(Noman->host[hostID].nomanNum != 0){
        /*�����Ӧ������������ֵ��*/
        printf("ID%d: | S-> %x | E-> %x | O-> %x |\r\n",hostID+1,Noman->host[hostID].ptrStart,Noman->host[hostID].ptrEnd,Noman->host[hostID].ptrOut);
        if(Noman->host[hostID].ptrOut < Noman->host[hostID].ptrEnd){
            /*ȡ����һ�κ���*/
            if((Noman->host[hostID].ptrOut-15) >= Noman->Start){
            	/*��ַ��1��Ϊ�ϴβ����IMEI*/
            	/*���ֻ���������ֵ�غ��״β���ȡ��ȡ���ϼ�IMEI����*/
            	if(Noman->host[hostID].ptrOut != Noman->host[3].ptrStart){
					cancel_imei(Noman->host[hostID].ptrOut - 15);
				}
            }else{
            	cancel_imei(Noman->Out);
			}
            /*����*/
            call_imei(Noman->host[hostID].ptrOut);
            /*�����˴β��ŵ�IMEI*/
            Noman->Out = Noman->host[hostID].ptrOut;
            /*��ַ����*/ 
            Noman->host[hostID].ptrOut += 15;
            if(Noman->host[hostID].ptrOut == Noman->host[hostID].ptrEnd)return 0;
            
            printf("ID%d: | O --> %x\r\n",hostID+1,Noman->host[hostID].ptrOut);
            return 0;
        	}else{
	            /*����ֵ�ز������*/
	            MyNomanCB.host[hostID].ptrOut = MyNomanCB.host[hostID].ptrStart;
	            return 1;
        	}        
    	}else{
        /*δ��������ֵ�ز����Ӧ����*/		
        if(hostID!=0){
            cancel_imei(Noman->Out);
        }
        
        /*���ļ�Ϊ�ּ�����ֵ�������˼��Ĳ�������*/
		if(hostID == 3)return 1;
		
    	call_imei(Noman->host[hostID].imei);
    	Noman->Out = Noman->host[hostID].imei;
    	printf("�������� Noman->Out: %x\r\n",Noman->Out);
        /*�����������*/
        return 1;
    }
}

void call_imei(char* imei)
{
	char temp[15];
	memcpy(temp,imei,15);
	if(temp[0] != 0x00){
		printf("Call IMEI: %s\r\n",temp);	
	}else{
		printf("�������\r\n");
	} 
}

void cancel_imei(char* imei)
{
	char temp[15];
	memcpy(temp,imei,15);
	if(imei[0] != 0x00){
		printf("Cancel call IMEI: %s\r\n",temp);	
	}else{
		printf("�������\r\n");
	}
}
