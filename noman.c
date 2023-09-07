/**
 * @file noman.c
 * @author viys (viys@yu.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-31
 * 
 * @copyright Copyright (c) 2023
 * 
 *	��������ʱ��ȥ�鿴����״̬������˫����ֱ�Ӻ�������ֵ��,ֱ����������ѯ���
 *	����������,���Ⱥ�������,����ѯ����ֵ��
 *	���ֻ�˫����ֱ�Ӻ�������ֵ��,��������������
 *
 *	����ֵ�غ�����ѵ����
 *
 *	����״̬�жϲ�������
 *
 *	����ֵ 0:δ��������,1: ��������,��δ��ѯ��� 2:��ѵ��� 3:������� 
 */

#include "noman.h"

/*���е�����ֵ��IMEI�����*/
char aNI[NOMAN_NUM][15];

/*����ֵ�ؿ��ƿ�*/
/*���ļ�Ϊ�ֻ�����ֵ��*/
Noman_CB MyNomanCB;
NomanAPI MyNomanAPI;

void noman_init(NomanAPI* API, Noman_CB* Noman)
{
    /*API�Խ�*/
    API->call_imei = my_call_noman;
    API->cancel_imei = my_cancel_noman;
	
	Noman->Last = NULL;
    Noman->Mutex = 0;
}

/**
 * @brief ȡ�����к���
 * 
 * @param imei IMEI
 * @param ch ͨ����
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
 * @brief ���к���
 * 
 * @param imei IMEI
 * @param ch ͨ����
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
 * @brief ����ֵ���ֲ�����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param hostID ����ID
 * @param ch ͨ����
 * @return uint8 0:δ��������,1: ��������,��δ��ѯ��� 2:��ѵ��� 3:������� 
 * @note �ڶ��ν����жϵ�ʱ��ֱ�Ӳ�����һ��
 */
static uint8 noman_recursion(Noman_CB* Noman, uint8 hostID, uint8 ch){
	uint8 ret = 0;
	/*����˺���,Ĭ�ϸ������°�������ֵ�غ���*/
	
	if(Noman->Out == Noman->End || Noman->host[3].ptrOut >= Noman->host[3].ptrEnd){
	/*ȫ�������ж�*/
		ret = 3;
	}else if(Noman->host[hostID].ptrOut < Noman->host[hostID].ptrEnd){
		/*�ü�����������ֵ�غ���δ��ѯ���*/
		
		/*������һ������ֵ�غ���*/
		call_imei(Noman->host[hostID].ptrOut, ch, Noman);
		
		/*��ַ��λ����*/
		Noman->Out = Noman->host[hostID].ptrOut;
		Noman->Last = Noman->Out;
		Noman->host[hostID].ptrOut += 15;
		/*����һ��*/
		ret = 1;
		
		if(Noman->host[hostID].ptrOut >= Noman->host[hostID].ptrEnd){
			/*������ѯ��ϱ�־λ����*/
			Noman->host[hostID].ptrOut == Noman->host[hostID].ptrStart;
			Noman->host[hostID].callStatus = Noman->host[hostID].nomanStatus;
			/*��ѯ���*/
			ret = 2;
		}
	}
	return ret;
}

/**
 * @brief ��������ֵ��״̬���ܽ�������
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param hostID ����ID
 * @return uint8 0:˫�� 1:���� 2:˫��
 */
static uint8 noman_host_judge(Noman_CB* Noman, uint8 hostID)
{
	int ret = 0; 
	if((Noman->host[hostID].nomanStatus == 1 || Noman->host[hostID].callStatus == 1) && Noman->host[hostID].nomanNum != 0){
		/*����ֵ��״̬��־λΪ1,����ֵ�ع��ܺ��벻Ϊ��*/
		/*˫��*/
		ret = 2;
	}else if(Noman->host[hostID].nomanStatus == 0 && Noman->host[hostID].nomanNum != 0){
		/*����ֵ��״̬��־λΪ0,����ֵ�ع��ܺ��벻Ϊ��*/
		/*����*/
		/*��ѯһ�ε�����,����״̬�´��ж�Ϊ˫��*/
		Noman->host[hostID].callStatus = 1;
		ret = 1;
	}else if(Noman->host[hostID].nomanNum == 0 || Noman->host[hostID].nomanStatus == 0){
		/*����ֵ��״̬��־λΪ0,����ֵ�ع��ܺ���Ϊ��,����������ֵ��״̬Ϊ1,���޹��ܺ���*/
		/*˫��*/
		ret = 0;
	}
	return ret;
}

/**
 * @brief ����ֵ�ز����߼�ѡ����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param hostID ����ID
 * @param ch ͨ����
 * @return uint8 0:error 1:�����в����� 2:������ѯ��� 3:ȫ����ѯ���
 */
static uint8 noman_host_branch(Noman_CB* Noman, uint8 hostID, uint8 ch)
{
	int ret = 0;
	
	switch(noman_host_judge(Noman, hostID)){
		case 0:
			/*˫��*/
			/*��������*/
			call_imei(Noman->host[hostID].imei, ch, Noman);
			/*˫��,ֱ�ӷ��ر����������*/
			ret = 2;
			break;
		case 1:
			/*����*/
			/*��������*/
			call_imei(Noman->host[hostID].imei, ch, Noman);
			/*������ѯ*/
			ret = 1;
			break;
		case 2:
			/*˫��*/
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
		/*����ֻ���������ֵ��*/ 
		ret = noman_host_branch(Noman,3, ch);
		if(ret==3){
			/*�ֺ����*/
			cancel_imei(Noman->Last, ch);
			printf("�ֺ����\r\n");
		}
	}else if(Noman->hostStatus<Noman->hostNum){
		/*��ֹĩ������˫��,�ظ�����*/
		ret = noman_host_branch(Noman, Noman->hostStatus, ch);
		if(ret==3){
			/*�ֺ����*/
			Noman->hostStatus = 0;
			cancel_imei(Noman->Last, ch);
			printf("�ֺ����\r\n");
		}else if(ret==2){
			Noman->hostStatus++;
			printf("ת��һ��\r\n");	
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
    /*��������������Ӧ������ֵ�غ�����*/
    ret = sscanf(data,"c%d.%d.%d.%d.",
                &Noman->host[0].nomanNum, &Noman->host[1].nomanNum, &Noman->host[2].nomanNum, &Noman->host[3].nomanNum);
    data += 9;

    /*��������ֵ���ܺ�����*/
    Noman->allNum = Noman->host[0].nomanNum + Noman->host[1].nomanNum + Noman->host[2].nomanNum + Noman->host[3].nomanNum;
    
    printf("Noman IMEI num: %d\r\n",Noman->allNum);
    printf("HostI num:%d,HostII num:%d,HostIII num:%d,Slave num:%d\r\n",Noman->host[0].nomanNum,Noman->host[1].nomanNum,Noman->host[2].nomanNum,Noman->host[3].nomanNum);
    
	/*��������ֵ��IMEI*/
    for(uint8 i=0; i<Noman->allNum; i++){
        ret = sscanf(data,"%[0-9],",aNI[i]);
        /**/
        data += strlen(aNI[i]) + 1;
        if(strlen(aNI[i])==11){
        	memcpy(&aNI[i][11],"0000",4);
		}
		
        printf("Noman ID: %d -> %s >> size:%d\r\n",i+1,aNI[i],strlen(aNI[i]));
    }

    /*��������ֵ�ص�ַ*/
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
    /*����ֵ�ز���ָ���λ*/
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
        /*����������*/
        Noman->Mutex = 1;
        printf("��������\r\n");
    }else{
        /*�رջ�����*/
        Noman->Mutex = 0;
        printf("�������ر�\r\n");
    }
}

uint8 noman_mutex_get(Noman_CB* Noman)
{
    return Noman->Mutex;
}

void my_call_noman(char* imei, uint8 ch)
{
	printf("����: %15.15s\r\n",imei);
}



void my_cancel_noman(char* imei, uint8 ch)
{
	printf("ȡ������: %15.15s\r\n",imei);
}

