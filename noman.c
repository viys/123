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

/*所有的无人值守IMEI存放区*/
char aNI[NOMAN_NUM][15];

/*无人值守控制块*/
/*第四级为分机无人值守*/
Noman_CB MyNomanCB;

uint8 noman_televise(Noman_CB Noman)
{
	uint8 ret = 0;
	if(MyNomanCB.host[3].nomanNum != 0){
		/*如果分机开启无人值守*/ 
		ret = call_noman(&MyNomanCB,3);
		if(ret==2){
			/*轮呼完毕*/
			printf("轮呼完毕\r\n");
		}
	}else{
		ret = call_noman(&MyNomanCB,MyNomanCB.hostStatus);
		if(ret==2){
			/*轮呼完毕*/
			MyNomanCB.hostStatus = 0;
			printf("轮呼完毕\r\n");
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
    /*解析各级主机对应的无人值守号码数*/
    ret = sscanf(data,"s%d.%d.%d.%d.",
                &MyNomanCB.host[0].nomanNum, &MyNomanCB.host[1].nomanNum, &MyNomanCB.host[2].nomanNum, &MyNomanCB.host[3].nomanNum);
    data += 9;

    /*计算无人值守总号码数*/
    MyNomanCB.allNum = MyNomanCB.host[0].nomanNum + MyNomanCB.host[1].nomanNum + MyNomanCB.host[2].nomanNum + MyNomanCB.host[3].nomanNum;
    
    printf("Noman IMEI num: %d\r\n",MyNomanCB.allNum);
    printf("HostI num:%d,HostII num:%d,HostIII num:%d,Slave num:%d\r\n",MyNomanCB.host[0].nomanNum,MyNomanCB.host[1].nomanNum,MyNomanCB.host[2].nomanNum,MyNomanCB.host[3].nomanNum);
    
	/*解析无人值守IMEI*/
    for(uint8 i=0; i<MyNomanCB.allNum; i++){
        ret = sscanf(data,"%[0-9],",aNI[i]);
        /**/
        data += strlen(aNI[i]) + 1;
        if(strlen(aNI[i])==11){
        	memcpy(&aNI[i][11],"0000",4);
		}
		
        printf("Noman ID: %d -> %s >> size:%d\r\n",i+1,aNI[i],strlen(aNI[i]));
    }

    /*载入无人值守地址*/
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

/*第二次进来判断的时候要直接拨打下一级*/
uint8 call_noman(Noman_CB* Noman, uint8 hostID)
{
    uint8 ret = 0;
    
    if((Noman->Out == Noman->End && Noman->hostStatus == Noman->hostNum) || (Noman->host[3].nomanNum != 0 && Noman->Out == Noman->host[3].ptrEnd-15) || Noman->Out == (char*)&(Noman->host[Noman->hostNum - 1].imei)){
			/*拨打至三级无人值守或多级呼叫的最后一级*/
			cancel_imei(Noman->Out);
			MyNomanCB.Out = MyNomanCB.Start;
			return 2;  
	}
	/*如果本级下次轮呼无效则立马调至下一级*/
//	if(Noman->host[hostID].ptrOut == Noman->host[hostID].ptrEnd && (Noman->hostStatus < Noman->hostNum)){
//    	Noman->hostStatus++;
//		hostID++;
//	}
	/*正常逻辑*/
	if(Noman->host[hostID].nomanNum != 0){
        /*如果对应主机开启无人值守*/
        printf("ID%d: | S-> %x | E-> %x | O-> %x |\r\n",hostID+1,Noman->host[hostID].ptrStart,Noman->host[hostID].ptrEnd,Noman->host[hostID].ptrOut);
        if(Noman->host[hostID].ptrOut < Noman->host[hostID].ptrEnd){
            /*取消上一次呼叫*/
            if((Noman->host[hostID].ptrOut-15) >= Noman->Start){
            	/*地址减1即为上次拨打的IMEI*/
            	/*当分机开启无人值守后首次拨打不取消取消上级IMEI呼叫*/
            	if(Noman->host[hostID].ptrOut != Noman->host[3].ptrStart){
					cancel_imei(Noman->host[hostID].ptrOut - 15);
				}
            }else{
            	cancel_imei(Noman->Out);
			}
            /*拨打*/
            call_imei(Noman->host[hostID].ptrOut);
            /*保留此次拨号的IMEI*/
            Noman->Out = Noman->host[hostID].ptrOut;
            /*地址递增*/ 
            Noman->host[hostID].ptrOut += 15;
            if(Noman->host[hostID].ptrOut == Noman->host[hostID].ptrEnd)return 0;
            
            printf("ID%d: | O --> %x\r\n",hostID+1,Noman->host[hostID].ptrOut);
            return 0;
        	}else{
	            /*无人值守拨打完毕*/
	            MyNomanCB.host[hostID].ptrOut = MyNomanCB.host[hostID].ptrStart;
	            return 1;
        	}        
    	}else{
        /*未开启无人值守拨打对应主机*/		
        if(hostID!=0){
            cancel_imei(Noman->Out);
        }
        
        /*第四级为分级无人值守跳过此级的拨打主机*/
		if(hostID == 3)return 1;
		
    	call_imei(Noman->host[hostID].imei);
    	Noman->Out = Noman->host[hostID].imei;
    	printf("拨打主机 Noman->Out: %x\r\n",Noman->Out);
        /*主机拨打完毕*/
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
		printf("号码出错\r\n");
	} 
}

void cancel_imei(char* imei)
{
	char temp[15];
	memcpy(temp,imei,15);
	if(imei[0] != 0x00){
		printf("Cancel call IMEI: %s\r\n",temp);	
	}else{
		printf("号码出错\r\n");
	}
}
