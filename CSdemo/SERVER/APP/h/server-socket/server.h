/**
 * @file:   server.h
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
#ifndef __SERVER_H__
#define __SERVER_H__
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
/*                 结构体定义                   */
/*----------------------------------------------*/
typedef struct NET_STATUS_T
{
	unsigned int nRiteBit;       //传输带宽 /kbps
	float nRealRttUs;     //网络延时 /ms
	unsigned int nLossFraction;  //丢包率   %
	unsigned long nFrameNum;     //接受的帧数
}NET_STATUS;

/*----------------------------------------------*/
/*                 函数定义                     */
/*----------------------------------------------*/
/**
 * @function:   SocketserverInit
 * @brief:      Socket服务端初始化
 * @param[in]:  unsigned int port		端口号
 * @param[in]:  char *pEthDev			网卡名称字符
 * @param[out]: int *pSocketHandle     	socket句柄
 * @return:     ERR                     错误码
 */
ERR SocketServerInit(unsigned int port, char *pEthDev, int *pSocketHandle);

 /**
 * @function:   SocketRecv
 * @brief:      socket接受数据
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[in]: 	int *pBuffer     	   		数据缓存
 * @param[in]: 	unsigned int len       		数据长度
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketRecv(int socketHandle);

#endif

