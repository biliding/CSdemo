/**
 * @file:   client.h
 * @note:   test
 * @brief:  服务端头文件
 * @author: dingjiayu
 * @date:   2021/12/7
 * @note:
 * @History:
   1.日    期:   2021/12/7
     作    者:   dingjiayu
     修改历史: 创建文件
 */
#ifndef __CLIENT_H__
#define __CLIENT_H__
/*----------------------------------------------*/
/*                 包含头文件                   */
/*----------------------------------------------*/

#include "basedef.h"
#include "err.h"
#include <sys/time.h>
#include "string.h"
#include <stdlib.h>
#include <malloc.h>

/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 函数定义                     */
/*----------------------------------------------*/
/**
 * @function:   SocketClientInit
 * @brief:      Socket客户端初始化
 * @param[in]:  char *pIpAddr			IP地址字符串
 * @param[in]:  unsigned int port		端口号
 * @param[in]:  char *pEthDev			网卡名称字符
 * @param[in]:  unsigned int timeout	连接超时时间
 * @param[out]: int *pSocketHandle     socket句柄
 * @return:     ERR                     错误码
 */
 ERR SocketClientInit(char *pIpAddr, unsigned int port, char *pEthDev, unsigned int timeout, int *pSoctetHandle);
 
 
  /**
 * @function:   SocketSend
 * @brief:      socket发送数据
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[in]: 	int *pBuffer     	   		数据缓存
 * @param[in]: 	unsigned int len       		数据长度
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketSend(int socketHandle, char *pBuffer, unsigned int len);

 /**
 * @function:   SocketClose
 * @brief:      关闭socket
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketClose(int socketHandle);

/**
 * @function:   EnPacket
 * @brief:      进行封包
 * @param[in/out]: 	char *SendBuf     		需要进行封装的buffer
 * @param[int]: 	int Send_Len			数据长度
 * @param[int/out]: 	int firstFlag		是否是第一帧
 * @return:     ERR							错误码
 */
ERR EnPacket(char *sendBuf, int send_Len, int *firstFlag);

#endif