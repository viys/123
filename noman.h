/**
 * @file noman.h
 * @author viys (viys@yu.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __NOMAN_H
#define __NOMAN_H

#include "stdio.h"
#include "def.h"
#include "string.h" 

#define NOMAN_NUM   15
typedef struct
{
    void (*call_imei)(char* imei, uint8 ch);
    void (*cancel_imei)(char* imei, uint8 ch);
    void (*hang_up)(char* imei, uint8 ch);
}NomanAPI;

typedef struct
{
    /*����IMEI*/
    char imei[15];
    /*������Ӧ��IMEI����*/
    int nomanNum;
    /*������Ӧ����ֵ��״̬*/
    uint8 nomanStatus;
    /*������������ֵ�ز�����״̬*/
    uint8 callStatus;
    /*����ֵ�������ַ*/
    char *ptrOut;
    /*����ֵ����ʼ��ַ*/
    char *ptrStart;
    /*����ֵ�ؽ�����ַ*/
    char *ptrEnd;
}ImeiHost;

typedef struct
{
    /*��������IMEI*/
    char imei[15];
    /*�������Ĳ���״̬*/
    uint8 status;
    /*Ӧ������imei����*/
    uint8 num;
}ImeiAid;

typedef struct{
    /*����IMEI��*/
    ImeiHost host[4];
    /*�󶨵���������*/
    uint8 hostNum;
    /*�����ֲ�λ��*/
    uint8 hostStatus;
    /*��IMEI����*/
    int allNum;
    /*����ֵ�غ���洢��ʼ��ַ��ʼ��ַ*/
    char *Start;
    /*����ֵ�غ���洢��ʼ��ַ������ַ*/
    char *End;
    /*����ֵ��IMEI�ϴβ��ŵ�ַ*/
    char *Out;
    char *Last;
    /*������*/
    uint8 Mutex;
    /*Ӧ�����Ŀ��ƿ�*/
    ImeiAid Aid;
}Noman_CB;

extern Noman_CB MyNomanCB;
extern NomanAPI MyNomanAPI;

/**
 * @brief ����ֵ�س�ʼ������
 * 
 * @param API API�������ƿ�
 * @param Noman ����ֵ�ؿ��ƿ�
 * @note ��Ҫ�Լ��ⲿ��д
 */
void noman_init(NomanAPI* API, Noman_CB* Noman);

/**
 * @brief ����IMEI����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param data �������
 * @return uint8 ���ؽ�������
 */
uint8 host_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief ����ֵ��IMEI����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param data ���ݽ��룬���ַ�ҪΪc
 * @return int 
 */
int noman_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief ����ֵ��״̬��������
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param data ���ݽ��룬���ַ�ҪΪs
 * @return int 
 */
int noman_status_parse(Noman_CB* Noman, char* data);

/**
 * @brief ��������imei��������
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param data 
 * @return int 1ON 0OFF
 */
int aid_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief ����ֵ��ת������
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @return uint8 0:δ��������,1: ��������,��δ��ѯ��� 2:��ѵ��� 3:������� 
 */
uint8 noman_televise(Noman_CB* Noman, uint8 ch);

/**
 * @brief ����ֵ��ָ���λ����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 */
void noman_clear(Noman_CB* Noman);

/**
 * @brief ����ֵ�ػ������л�����
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @param flag 1ON 0OFF
 */
void noman_mutex_sw(Noman_CB* Noman, uint8 flag);

/**
 * @brief ��ȡ������
 * 
 * @param Noman ����ֵ�ؿ��ƿ�
 * @return uint8 0:�������ر� 1:��������
 */
uint8 noman_mutex_get(Noman_CB* Noman);

/**
 * @brief ��Ҫ�Լ���д���ⲿ�ӿں���
 * 
 */
void my_cancel_noman(char* imei, uint8 ch);
void my_call_noman(char* imei, uint8 ch);

#endif

