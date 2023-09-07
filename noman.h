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
    /*主机IMEI*/
    char imei[15];
    /*主机对应的IMEI数量*/
    int nomanNum;
    /*主机对应无人值守状态*/
    uint8 nomanStatus;
    /*主机单关无人值守拨主机状态*/
    uint8 callStatus;
    /*无人值守输出地址*/
    char *ptrOut;
    /*无人值守起始地址*/
    char *ptrStart;
    /*无人值守结束地址*/
    char *ptrEnd;
}ImeiHost;

typedef struct
{
    /*急救中心IMEI*/
    char imei[15];
    /*急救中心拨号状态*/
    uint8 status;
    /*应急中心imei数量*/
    uint8 num;
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
    /*无人值守号码存储起始地址起始地址*/
    char *Start;
    /*无人值守号码存储起始地址结束地址*/
    char *End;
    /*无人值守IMEI上次拨号地址*/
    char *Out;
    char *Last;
    /*互斥量*/
    uint8 Mutex;
    /*应急中心控制块*/
    ImeiAid Aid;
}Noman_CB;

extern Noman_CB MyNomanCB;
extern NomanAPI MyNomanAPI;

/**
 * @brief 无人值守初始化函数
 * 
 * @param API API函数控制块
 * @param Noman 无人值守控制块
 * @note 需要自己外部编写
 */
void noman_init(NomanAPI* API, Noman_CB* Noman);

/**
 * @brief 主机IMEI解析
 * 
 * @param Noman 无人值守控制块
 * @param data 解析入口
 * @return uint8 返回解析数量
 */
uint8 host_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief 无人值守IMEI解析
 * 
 * @param Noman 无人值守控制块
 * @param data 数据接入，首字符要为c
 * @return int 
 */
int noman_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief 无人值守状态解析函数
 * 
 * @param Noman 无人值守控制块
 * @param data 数据接入，首字符要为s
 * @return int 
 */
int noman_status_parse(Noman_CB* Noman, char* data);

/**
 * @brief 急救中心imei解析函数
 * 
 * @param Noman 无人值守控制块
 * @param data 
 * @return int 1ON 0OFF
 */
int aid_imei_parse(Noman_CB* Noman, char *data);

/**
 * @brief 无人值守转播函数
 * 
 * @param Noman 无人值守控制块
 * @return uint8 0:未产生拨打,1: 产生拨打,但未轮询完毕 2:轮训完毕 3:拨打结束 
 */
uint8 noman_televise(Noman_CB* Noman, uint8 ch);

/**
 * @brief 无人值守指针归位函数
 * 
 * @param Noman 无人值守控制块
 */
void noman_clear(Noman_CB* Noman);

/**
 * @brief 无人值守互斥量切换函数
 * 
 * @param Noman 无人值守控制块
 * @param flag 1ON 0OFF
 */
void noman_mutex_sw(Noman_CB* Noman, uint8 flag);

/**
 * @brief 获取互斥量
 * 
 * @param Noman 无人值守控制块
 * @return uint8 0:互斥量关闭 1:互斥量打开
 */
uint8 noman_mutex_get(Noman_CB* Noman);

/**
 * @brief 需要自己编写的外部接口函数
 * 
 */
void my_cancel_noman(char* imei, uint8 ch);
void my_call_noman(char* imei, uint8 ch);

#endif

