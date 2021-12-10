/**
 * @file:   client.cpp
 * @note:  	test
 * @brief:  客户端功能代码
 * @author: 
 * @date:   2021/12/6
 * @note:
 * @History:
   1.日    期:   2021/12/6
     作    者:   dingjiayu
     修改历史: 创建文件
 */

/*----------------------------------------------*/
/*                 包含头文件                   */
/*----------------------------------------------*/
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include "basedef.h"
#include "err.h"
#include "client.h"

/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/


/*----------------------------------------------*/
/*                 结构体定义                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 函数声明                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 全局变量                     */
/*----------------------------------------------*/

unsigned long Frame_id = 0;

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
 ERR SocketClientInit(char *pIpAddr, unsigned int port, char *pEthDev, unsigned int timeOut, int *pSocketHandle)
{
	struct ifreq interface;
    struct sockaddr_in socketAddr;
    int ret = 0;
    struct timeval strTimeval;

	//参数检查
    if ((pEthDev == NULL)||(pSocketHandle == NULL))
    {
        DEBUG("invalid param");
        return ERR_COM_INVALID_PARAM;
    }
	//创建套接字
    *pSocketHandle = socket(AF_INET, SOCK_STREAM, 0);
    if (*pSocketHandle < 0)
    {
    	DEBUG("socket()FAILED_INIT!!!");
        return ERR_COM_FAILED_INIT;
    }
    memset((void *)&interface, 0x0, sizeof(interface));
	strncpy(interface.ifr_ifrn.ifrn_name, pEthDev, sizeof(interface.ifr_ifrn.ifrn_name)-1);
	//绑定网卡
    //ret = setsockopt(*pSocketHandle, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface));
    if (ret < 0)
    {
    	DEBUG("setsockopt()FAILED_INIT!!!");
        close(*pSocketHandle);
        return ERR_COM_FAILED_INIT;
    }

    //尝试连接
    do
    {
        socketAddr.sin_family = AF_INET;
    	socketAddr.sin_addr.s_addr = inet_addr(pIpAddr);
    	socketAddr.sin_port = htons(port);
        ret = connect(*pSocketHandle, (struct sockaddr*)&socketAddr, sizeof(struct sockaddr_in));
        if (ret != 0)
        {
            usleep(1000*1000);
        }
    } while (ret != 0);
	
   	//设置接受发送的超时时间
	memset(&strTimeval, 0x0, sizeof(strTimeval));
	strTimeval.tv_sec = timeOut;
	strTimeval.tv_usec = 0;
	//设置发送超时
	ret=setsockopt(*pSocketHandle,SOL_SOCKET,SO_SNDTIMEO,(const char*)&strTimeval,sizeof(struct timeval));
	//设置接受超时
	ret=setsockopt(*pSocketHandle,SOL_SOCKET,SO_RCVTIMEO,(const char*)&strTimeval,sizeof(struct timeval)); 
    if(ret != 0)
    {
    	DEBUG("setsockopt FAILED_PROCESS");
    	return ERR_COM_FAILED_INIT;
    }	

    RET_OK;
}

 /**
 * @function:   SocketSend
 * @brief:      socket发送数据
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[in]: 	int *pBuffer     	   		数据缓存
 * @param[in]: 	unsigned int len       		数据长度
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketSend(int socketHandle, char *pBuffer, unsigned int len)
 {
 	//发送长度
 	int sendLen = 0;
    int ret = 0;
	
	//入参检查
    if ((socketHandle == 0)
        || (pBuffer == NULL)
        || (len == 0))
    {     
    	DEBUG("invalid param!!! sockfd:%d, pBuf:%p, len:%d",socketHandle, pBuffer, len);
        return ERR_COM_INVALID_PARAM;
    }
    
    do
    {
        ret = send(socketHandle, pBuffer, len, 0);
		if((ret == -1) && (errno == EAGAIN))
    	{
    		DEBUG("connect timeout");
    		return ERR_COM_TIME_OUT;
    	}
    	else if(ret <= 0)
    	{
    		DEBUG("connect break");
    		return ERR_COM_CONNECT_BREAK;
    	}
        sendLen += ret;
    } while ((sendLen < len));

    RET_OK;
 }

 /**
 * @function:   SocketClose
 * @brief:      关闭socket
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketClose(int socketHandle)
{
	close(socketHandle);
	RET_OK;
}

/**
 * @function:   EnPacket
 * @brief:      进行封包
 * @param[in/out]: 	char *SendBuf     		需要进行封装的buffer
 * @param[int]: 	int Send_Len			数据长度
 * @param[int/out]: 	int firstFlag		是否是第一帧
 * @return:     ERR							错误码
 */
ERR EnPacket(char *sendBuf, int send_Len, int *firstFlag)
{
	//入参检查
	if((NULL == sendBuf)||(send_Len < 10))
	{
			return ERR_COM_INVALID_PARAM;
	}	
	if(1 == *firstFlag)
	{
		Frame_id = 0;
		*firstFlag = 0;
	}
	*(unsigned long *)sendBuf = htonl(Frame_id);
	*(unsigned short *)(sendBuf + 8) = htons((unsigned short)send_Len);
	if(Frame_id >= 0xFFFFFFFF)
	{
		Frame_id = 0;
	}
	else
	{
		Frame_id++;
	}
	RET_OK;
}