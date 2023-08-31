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

#define NOMAN_NUM   15

typedef struct
{
    /*����IMEI*/
    char imei[15];
    /*������Ӧ��IMEI����*/
    int nomanNum;
    /*�����ַ*/
    char *ptrOut;
    /*��ʼ��ַ*/
    char *ptrStart;
    /*������ַ*/
    char *ptrEnd;
}ImeiHost;

typedef struct
{
    /*��������IMEI*/
    char aidImei[15];
    /*��������״̬*/
    uint8 aidStatus;
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
    /*��ʼ��ַ*/
    char *Start;
    /*������ַ*/
    char *End;
    /*�ϴ�IMEI���ŵ�ַ*/
    char *Out;
}Noman_CB;

extern Noman_CB MyNomanCB;

/**
 * @brief ����ֵ��IMEI����
 * 
 * @param data ���ݽ��룬���ַ�ҪΪs
 * @return int ���ػ�ȡ����
 */
int noman_imei_parse(char *data);

/**
 * @brief ����IMEI����
 * 
 * @param data ���ݽ��룬���ַ�ҪΪs
 * @return uint8 ���ػ�ȡ����
 */
uint8 host_imei_parse(char *data);

/**
 * @brief ����ֵ��ת������
 * 
 * @param Noman ����ֵ�ؿ��ƿ� 0�������������ֺ� 1�����ֺ���� 2ȫ����� 
 */
uint8 noman_televise(Noman_CB Noman);

/**
 * @brief ����ֵ�ز��ź���
 * 
 * @param *Noman ����ֵ�ؿ��ƿ�
 * @param hostID ����ID
 * @return uint8 �����Ƿ���ѵ����Ӧ���������һ������
 */
uint8 call_noman(Noman_CB* Noman, uint8 hostID);

/**
 * @brief ���к���
 * 
 * @param imei 
 */
void call_imei(char* imei);

/**
 * @brief ȡ�����к���
 * 
 * @param imei 
 */
void cancel_imei(char* imei);

#endif

