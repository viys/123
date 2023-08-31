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
    /*主机IMEI*/
    char imei[15];
    /*主机对应的IMEI数量*/
    int nomanNum;
    /*输出地址*/
    char *ptrOut;
    /*起始地址*/
    char *ptrStart;
    /*结束地址*/
    char *ptrEnd;
}ImeiHost;

typedef struct
{
    /*急救中心IMEI*/
    char aidImei[15];
    /*急救中心状态*/
    uint8 aidStatus;
}ImeiAid;

typedef struct{
    /*主机IMEI集*/
    ImeiHost host[4];
    /*绑定的主机数量*/
    uint8 hostNum;
    /*主机轮播位置*/
    uint8 hostStatus;
    /*总IMEI数量*/
    int allNum;
    /*起始地址*/
    char *Start;
    /*结束地址*/
    char *End;
    /*上次IMEI拨号地址*/
    char *Out;
}Noman_CB;

extern Noman_CB MyNomanCB;

/**
 * @brief 无人值守IMEI解析
 * 
 * @param data 数据接入，首字符要为s
 * @return int 返回获取数量
 */
int noman_imei_parse(char *data);

/**
 * @brief 主机IMEI解析
 * 
 * @param data 数据接入，首字符要为s
 * @return uint8 返回获取数量
 */
uint8 host_imei_parse(char *data);

/**
 * @brief 无人值守转播函数
 * 
 * @param Noman 无人值守控制块 0本级主机无人轮呼 1本级轮呼完毕 2全部完成 
 */
uint8 noman_televise(Noman_CB Noman);

/**
 * @brief 无人值守拨号函数
 * 
 * @param *Noman 无人值守控制块
 * @param hostID 主机ID
 * @return uint8 返回是否轮训到对应主机的最后一个号码
 */
uint8 call_noman(Noman_CB* Noman, uint8 hostID);

/**
 * @brief 呼叫函数
 * 
 * @param imei 
 */
void call_imei(char* imei);

/**
 * @brief 取消呼叫函数
 * 
 * @param imei 
 */
void cancel_imei(char* imei);

#endif

