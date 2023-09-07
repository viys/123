/**
 * @file noman.c
 * @author viys (viys@yu.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-31
 * 
 * @copyright Copyright (c) 2023
 * 
 *	产生呼叫时先去查看主机状态若主机双开则直接呼叫无人值守,直至本主机轮询完毕
 *	若主机单开,则先呼叫主机,再轮询无人值守
 *	若分机双开则直接呼叫无人值守,不呼叫其他主机
 *
 *	无人值守号码轮训函数
 *
 *	主机状态判断并拨打函数
 *
 *	返回值 0:未产生拨打,1: 产生拨打,但未轮询完毕 2:轮训完毕 3:拨打结束 
 */

#include "noman.h"

/*所有的无人值守IMEI存放区*/
char aNI[NOMAN_NUM][15];

/*无人值守控制块*/
/*第四级为分机无人值守*/
Noman_CB MyNomanCB;
NomanAPI MyNomanAPI;

void noman_init(NomanAPI* API, Noman_CB* Noman)
{
    /*API对接*/
    API->call_imei = my_call_noman;
    API->cancel_imei = my_cancel_noman;
	
	Noman->Last = NULL;
    Noman->Mutex = 0;
}

/**
 * @brief 取消呼叫函数
 * 
 * @param imei IMEI
 * @param ch 通道号
 */
static void cancel_imei(char* imei, uint8 ch)
{
	if(imei != NULL){
        if(MyNomanAPI.cancel_imei != NULL){
            MyNomanAPI.cancel_imei(imei, ch);
        }
	}
}

/**
 * @brief 呼叫函数
 * 
 * @param imei IMEI
 * @param ch 通道号
 */
static void call_imei(char* imei, uint8 ch, Noman_CB* Noman)
{
	cancel_imei(Noman->Last, ch);
    if(imei != NULL){
        if(MyNomanAPI.call_imei != NULL){
            MyNomanAPI.call_imei(imei, ch);
        }
	}
	Noman->Last = imei;
}

/**
 * @brief 无人值守轮播函数
 * 
 * @param Noman 无人值守控制块
 * @param hostID 主机ID
 * @param ch 通道号
 * @return uint8 0:未产生拨打,1: 产生拨打,但未轮询完毕 2:轮训完毕 3:拨打结束 
 * @note 第二次进来判断的时候直接拨打下一级
 */
static uint8 noman_recursion(Noman_CB* Noman, uint8 hostID, uint8 ch){
	uint8 ret = 0;
	/*进入此函数,默认该主机下帮有无人值守号码*/
	
	if(Noman->Out == Noman->End || Noman->host[3].ptrOut >= Noman->host[3].ptrEnd){
	/*全部播完判断*/
		ret = 3;
	}else if(Noman->host[hostID].ptrOut < Noman->host[hostID].ptrEnd){
		/*该级主机下无人值守号码未轮询完毕*/
		
		/*呼叫下一个无人值守号码*/
		call_imei(Noman->host[hostID].ptrOut, ch, Noman);
		
		/*地址移位处理*/
		Noman->Out = Noman->host[hostID].ptrOut;
		Noman->Last = Noman->Out;
		Noman->host[hostID].ptrOut += 15;
		/*拨打一次*/
		ret = 1;
		
		if(Noman->host[hostID].ptrOut >= Noman->host[hostID].ptrEnd){
			/*本级轮询完毕标志位处理*/
			Noman->host[hostID].ptrOut == Noman->host[hostID].ptrStart;
			Noman->host[hostID].callStatus = Noman->host[hostID].nomanStatus;
			/*轮询完毕*/
			ret = 2;
		}
	}
	return ret;
}

/**
 * @brief 主机无人值守状态功能解析函数
 * 
 * @param Noman 无人值守控制块
 * @param hostID 主机ID
 * @return uint8 0:双关 1:单开 2:双开
 */
static uint8 noman_host_judge(Noman_CB* Noman, uint8 hostID)
{
	int ret = 0; 
	if((Noman->host[hostID].nomanStatus == 1 || Noman->host[hostID].callStatus == 1) && Noman->host[hostID].nomanNum != 0){
		/*无人值守状态标志位为1,无人值守功能号码不为零*/
		/*双开*/
		ret = 2;
	}else if(Noman->host[hostID].nomanStatus == 0 && Noman->host[hostID].nomanNum != 0){
		/*无人值守状态标志位为0,无人值守功能号码不为零*/
		/*单开*/
		/*查询一次单开后,主机状态下次判断为双开*/
		Noman->host[hostID].callStatus = 1;
		ret = 1;
	}else if(Noman->host[hostID].nomanNum == 0 || Noman->host[hostID].nomanStatus == 0){
		/*无人值守状态标志位为0,无人值守功能号码为零,并包含无人值守状态为1,但无功能号码*/
		/*双关*/
		ret = 0;
	}
	return ret;
}

/**
 * @brief 无人值守拨号逻辑选择函数
 * 
 * @param Noman 无人值守控制块
 * @param hostID 主机ID
 * @param ch 通道数
 * @return uint8 0:error 1:呼叫中部号码 2:单级轮询完成 3:全部轮询完成
 */
static uint8 noman_host_branch(Noman_CB* Noman, uint8 hostID, uint8 ch)
{
	int ret = 0;
	
	switch(noman_host_judge(Noman, hostID)){
		case 0:
			/*双关*/
			/*呼叫主机*/
			call_imei(Noman->host[hostID].imei, ch, Noman);
			/*双关,直接返回本级拨打完毕*/
			ret = 2;
			break;
		case 1:
			/*单开*/
			/*呼叫主机*/
			call_imei(Noman->host[hostID].imei, ch, Noman);
			/*继续轮询*/
			ret = 1;
			break;
		case 2:
			/*双开*/
			ret = noman_recursion(Noman, hostID, ch);
			break;
		default:
			break;
	}

	return ret;
}

uint8 noman_televise(Noman_CB* Noman, uint8 ch)
{
    uint8 ret = 0;
	
	if(Noman->host[3].nomanNum != 0 && Noman->host[3].nomanStatus != 0){
		/*如果分机开启无人值守*/ 
		ret = noman_host_branch(Noman,3, ch);
		if(ret==3){
			/*轮呼完毕*/
			cancel_imei(Noman->Last, ch);
			printf("轮呼完毕\r\n");
		}
	}else if(Noman->hostStatus<Noman->hostNum){
		/*防止末级主机双关,重复进入*/
		ret = noman_host_branch(Noman, Noman->hostStatus, ch);
		if(ret==3){
			/*轮呼完毕*/
			Noman->hostStatus = 0;
			cancel_imei(Noman->Last, ch);
			printf("轮呼完毕\r\n");
		}else if(ret==2){
			Noman->hostStatus++;
			printf("转下一级\r\n");	
		}
	}else{
		cancel_imei(Noman->Last, ch);
		ret = 3;
	}
	return ret;
}

int aid_imei_parse(Noman_CB* Noman, char *data)
{
	Noman->Aid.num = sscanf(data,"1%[0-9],",Noman->Aid.imei);
	if(strlen(Noman->Aid.imei)==11){
		memcpy(&Noman->Aid.imei[11],"0000",4);
	}
	return Noman->Aid.num;
}

int noman_status_parse(Noman_CB* Noman, char* data)
{
    uint8 ret = 0;
    ret = sscanf(data,"s%d.%d.%d.%d",&Noman->host[0].nomanStatus,&Noman->host[1].nomanStatus,&Noman->host[2].nomanStatus,&Noman->host[3].nomanStatus);
    printf("NOMAN STAUS HostI:%d HostII:%d HostIII:%d Slave:%d\r\n",Noman->host[0].nomanStatus,Noman->host[1].nomanStatus,Noman->host[2].nomanStatus,Noman->host[3].nomanStatus);
    return ret;
}

int noman_imei_parse(Noman_CB* Noman, char *data)
{
    uint8 ret = 0;
    memset(aNI,0x00,sizeof(aNI));
    /*解析各级主机对应的无人值守号码数*/
    ret = sscanf(data,"c%d.%d.%d.%d.",
                &Noman->host[0].nomanNum, &Noman->host[1].nomanNum, &Noman->host[2].nomanNum, &Noman->host[3].nomanNum);
    data += 9;

    /*计算无人值守总号码数*/
    Noman->allNum = Noman->host[0].nomanNum + Noman->host[1].nomanNum + Noman->host[2].nomanNum + Noman->host[3].nomanNum;
    
    printf("Noman IMEI num: %d\r\n",Noman->allNum);
    printf("HostI num:%d,HostII num:%d,HostIII num:%d,Slave num:%d\r\n",Noman->host[0].nomanNum,Noman->host[1].nomanNum,Noman->host[2].nomanNum,Noman->host[3].nomanNum);
    
	/*解析无人值守IMEI*/
    for(uint8 i=0; i<Noman->allNum; i++){
        ret = sscanf(data,"%[0-9],",aNI[i]);
        /**/
        data += strlen(aNI[i]) + 1;
        if(strlen(aNI[i])==11){
        	memcpy(&aNI[i][11],"0000",4);
		}
		
        printf("Noman ID: %d -> %s >> size:%d\r\n",i+1,aNI[i],strlen(aNI[i]));
    }

    /*载入无人值守地址*/
    Noman->hostStatus = 0;
    Noman->Start = aNI[0];
    Noman->Out = Noman->Start;
    Noman->End = Noman->Start + (Noman->allNum-Noman->host[3].nomanNum-1)*15;
    Noman->host[0].ptrStart = Noman->Start;
    Noman->host[0].ptrOut = Noman->host[0].ptrStart;
    Noman->host[0].ptrEnd = Noman->host[0].ptrStart + Noman->host[0].nomanNum*15;
    for(uint8 i=1; i<4; i++){
        Noman->host[i].ptrStart = Noman->host[i-1].ptrEnd;
        Noman->host[i].ptrOut = Noman->host[i].ptrStart;
        Noman->host[i].ptrEnd = Noman->host[i].ptrStart + Noman->host[i].nomanNum*15;
    }
    return Noman->allNum;
}

uint8 host_imei_parse(Noman_CB* Noman, char *data)
{
    Noman->hostNum = sscanf(data,"%[0-9],%[0-9],%[0-9],",Noman->host[0].imei,Noman->host[1].imei,Noman->host[2].imei);
    printf("HOST I:%s, HOST II:%s, HOSTI III:%s\r\n",Noman->host[0].imei,Noman->host[1].imei,Noman->host[2].imei);
    return Noman->hostNum;
}

void noman_clear(Noman_CB* Noman)
{
    /*无人值守拨号指针归位*/
    Noman->hostStatus = 0;
    for(int i=0; i<3; i++){
    	Noman->host[i].callStatus = Noman->host[i].nomanStatus;
	}
	
    Noman->host[0].ptrStart = Noman->Start;
    Noman->host[0].ptrOut = Noman->host[0].ptrStart;
    Noman->host[0].ptrEnd = Noman->host[0].ptrStart + Noman->host[0].nomanNum*15;
    for(uint8 i=1; i<4; i++){
        Noman->host[i].ptrStart = Noman->host[i-1].ptrEnd;
        Noman->host[i].ptrOut = Noman->host[i].ptrStart;
        Noman->host[i].ptrEnd = Noman->host[i].ptrStart + Noman->host[i].nomanNum*15;
    }
}

void noman_mutex_sw(Noman_CB* Noman, uint8 flag)
{
    if(flag!=0){
        /*开启互斥量*/
        Noman->Mutex = 1;
        printf("互斥量打开\r\n");
    }else{
        /*关闭互斥量*/
        Noman->Mutex = 0;
        printf("互斥量关闭\r\n");
    }
}

uint8 noman_mutex_get(Noman_CB* Noman)
{
    return Noman->Mutex;
}

void my_call_noman(char* imei, uint8 ch)
{
	printf("呼叫: %15.15s\r\n",imei);
}



void my_cancel_noman(char* imei, uint8 ch)
{
	printf("取消呼叫: %15.15s\r\n",imei);
}

