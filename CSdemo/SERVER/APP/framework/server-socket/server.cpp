/**
 * @file:   server.cpp
 * @note:  	test
 * @brief:  服务端功能代码
 * @author: dingjiayu
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
#include <sys/epoll.h>
#include "server.h"


/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/
//最大监听数量
#define    MAXEVENTS			32
#define    HLEN   				10
#define	   MAXNUM   			1024  
#define    CLCLEN               10000
#define    MTU   				1500
	
/*----------------------------------------------*/
/*                 结构体定义                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 函数声明                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 全局变量                     */
/*----------------------------------------------*/
unsigned char DelayTime[8];


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
ERR SocketServerInit(unsigned int port, char *pEthDev, int *pSocketHandle)
	{
		struct ifreq interface;
		struct sockaddr_in socketAddr;
		int ret = 0;
		int reUse = 0;
		
		if ((pEthDev == NULL)
			|| (pSocketHandle == NULL))
		{
			DEBUG("pEthDev == NULL!!!");
			return ERR_COM_INVALID_PARAM;
		}
		//创建套接字
		*pSocketHandle = socket(AF_INET, SOCK_STREAM, 0);
		if (*pSocketHandle < 0)
		{
			DEBUG("socket() FAILED_INIT!!!");
			return ERR_COM_FAILED_INIT;
		}
		//绑定网卡
		memset((void *)&interface, 0x0, sizeof(interface));
		strncpy(interface.ifr_ifrn.ifrn_name, pEthDev, sizeof(interface.ifr_ifrn.ifrn_name)-1);
		//ret = setsockopt(*pSocketHandle, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface));
		if (ret < 0)
		{
			DEBUG("setsockopt() FAILED_INIT!!!");
			close(*pSocketHandle);
			return ERR_COM_FAILED_INIT;
		}
		
		//设置端口复用
		reUse = 1;
		ret = setsockopt(*pSocketHandle, SOL_SOCKET, SO_REUSEADDR, (char *)&reUse, sizeof(reUse));
		if (ret < 0)
		{ 
			DEBUG("setsockopt() FAILED_INIT!!!");
			close(*pSocketHandle);
			return ERR_COM_FAILED_INIT;
		} 
		//设置延时一定时间关闭
		struct linger lin;
		lin.l_onoff = 1;
		lin.l_linger = 1;
		ret = setsockopt(*pSocketHandle, SOL_SOCKET, SO_LINGER, (char *)&lin, sizeof(lin));
		//绑定套接字
		memset((void *)&socketAddr, 0, sizeof(struct sockaddr_in));
		socketAddr.sin_family = AF_INET;
		socketAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
		socketAddr.sin_port = htons(port);
		bzero(&(socketAddr.sin_zero), 8);
		ret = bind(*pSocketHandle, (struct sockaddr *)&socketAddr, sizeof(struct sockaddr_in));
		if (ret < 0)
		{
			DEBUG("bind() FAILED_INIT!!!");
			close(*pSocketHandle);
			return ERR_COM_FAILED_INIT;
		}
		//开启监听
		ret = listen(*pSocketHandle, SOMAXCONN);
		if (ret < 0)
		{
			DEBUG("listen() FAILED_INIT!!!");
			close(*pSocketHandle);
			return ERR_COM_FAILED_INIT;
		}

		RET_OK;
	}

/**
 * @function:   Time_Current
 * @brief:      获取系统时间函数
 * @return:     unsigned long
**/
unsigned long Time_Current(void)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	gettimeofday(&tv, NULL);
	return (unsigned long)((unsigned long)tv.tv_sec * 1000000 + tv.tv_usec);
}
	
 /**
 * @function:   InitEpoll
 * @brief:      创建epoll
 * @param[in]: 	int fd     					需要监听的fd
 * @param[in/out]: 	epoll_event *Event			需要监听的事件
 * @param[out]: int tfd     					返回值
 * @return:     ERR                     	错误码
 */
ERR InitEpoll(int fd, struct epoll_event *Event,int *tfd)
{	
	int nRet;
	*tfd = epoll_create1 (0);
	if (-1 == *tfd)
	{
		DEBUG("epoll_create err!");
		return ERR_COM_EPOLL_INIT;
	}
	Event->data.fd = fd;
	Event->events = EPOLLIN | EPOLLET;//读入,边缘触发方式
	nRet = epoll_ctl(*tfd, EPOLL_CTL_ADD, fd, Event);
	if(-1 == nRet)
	{
		DEBUG("epoll_ctrl err!");
		return ERR_COM_EPOLL_INIT;
	}
	return OK;
}

 /**
 * @function:   SocketRecv
 * @brief:      socket接受数据
 * @param[in]: 	int socketHandle     		socket句柄
 * @param[in]: 	int *pBuffer     	   		数据缓存
 * @param[in]: 	unsigned int len       		数据长度
 * @param[out]: 	NONE
 * @return:     ERR                     	错误码
 */
ERR SocketRecv(int socketHandle)
{
	int efd;
	int nRet = 0;
	int idx;
	int First_Stream = 0;
	unsigned short Buffer_Len= 0;
	char buf[HLEN];
	char nbuf[MAXNUM];
	unsigned long Current_Time = 0;
	unsigned long Tmp_Time = 0;
	//计算传输包的数量
	unsigned int SCnt = 0;
	NET_STATUS NetStatus;
	struct epoll_event event;
	struct epoll_event *events;
	//入参检测
	if (socketHandle == 0)
	{
		DEBUG("invalid param");
		return ERR_COM_INVALID_PARAM;
	}

	nRet = InitEpoll(socketHandle, &event, &efd);
	if(OK != nRet)
	{
		DEBUG("init epoll err!");
		return nRet;
	}
	//分配epoll数量
	events = (epoll_event*)calloc (MAXEVENTS, sizeof(event));
	NetStatus.nFrameNum = 0;
	while(1)
	{
		int epollNum;
		First_Stream = 0;
		epollNum = epoll_wait (efd, events, MAXEVENTS, -1);
		for(idx = 0; idx < epollNum; idx++)
		{
			if ((events[idx].events & EPOLLERR) ||              
				(events[idx].events & EPOLLHUP) ||             
				(!(events[idx].events & EPOLLIN)))           
			{
				DEBUG("epoll error!");
				close(events[idx].data.fd);
				continue;
			}
			else if(socketHandle == events[idx].data.fd)
			{
				struct sockaddr in_addr;
				socklen_t in_len;
				in_len = sizeof(in_addr);
				int infd;
				infd = accept (socketHandle, &in_addr, &in_len);
				if(-1 == infd)
				{
					if((errno == EAGAIN)||(errno == EWOULDBLOCK))
					{
						break;
					}
					else
					{
						DEBUG("accept");
						break;
					}
				}
				event.data.fd = infd;
				event.events = EPOLLIN | EPOLLET;
				nRet = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
				if(-1 == nRet)
				{
					DEBUG("epoll ctrl error");
					return ERR_COM_EPOLL_CTRL;
				}
			}
			else
			{
				int nready = 0;
				//时间计时序号
				int TCnt = 0;
				int TmpCnt = 0;
				SCnt = 0;
				NetStatus.nFrameNum = 0;
				Current_Time = Time_Current();
				while(1)
				{
					ssize_t count;
					Tmp_Time = Time_Current();
					if(Tmp_Time >= Current_Time)
					{
						if((Tmp_Time - Current_Time) >= 1000000)
						{
							//DEBUG("1 second pass!");
							
							NetStatus.nRiteBit = (SCnt - TmpCnt) * Buffer_Len;
							//DEBUG("SCnt is %d",SCnt);
							//DEBUG("NetStatus.nFrameNum is %d ",NetStatus.nFrameNum);
							if(NetStatus.nFrameNum <= 0)
							{
								NetStatus.nLossFraction = 0xFFFF;
							}
							else
							{
								NetStatus.nLossFraction = (NetStatus.nFrameNum + 1 - SCnt) * 100 / NetStatus.nFrameNum;
							}
							if((SCnt - TmpCnt) > 0)
							{
								NetStatus.nRealRttUs = (float)((Tmp_Time - Current_Time) / 1000) / (float)NetStatus.nRiteBit * (float)MTU;
							}
							else
							{
								NetStatus.nRealRttUs = 0.0f;
							}
							DEBUG("nRiteBit is %d bKps",NetStatus.nRiteBit / 1000);
							DEBUG("nLossFraction is %d",NetStatus.nLossFraction);
							DEBUG("nRealRttUs is %f ms",NetStatus.nRealRttUs);
							TmpCnt = SCnt;
							Current_Time = Tmp_Time;
						}
					}
					else
					{
						if((Tmp_Time + 0xFFFFFFFF^(~Current_Time)) >= 1000000)
						{
							NetStatus.nRiteBit = (SCnt - TmpCnt) * Buffer_Len;
							//DEBUG("SCnt is %d",SCnt);
							//DEBUG("NetStatus.nFrameNum is %d bKps",NetStatus.nFrameNum/1000);
							if(NetStatus.nFrameNum <= 0)
							{
								NetStatus.nLossFraction = 0xFFFF;
							}
							else
							{
								NetStatus.nLossFraction = (NetStatus.nFrameNum + 1 - SCnt) * 100 / NetStatus.nFrameNum;
							}
							NetStatus.nRealRttUs = (float)((Tmp_Time + 0xFFFFFFFF^(~Current_Time)) / 1000) / (float)NetStatus.nRiteBit * (float)MTU;
							DEBUG("nRiteBit is %d",NetStatus.nRiteBit);
							DEBUG("nLossFraction is %d",NetStatus.nLossFraction);
							DEBUG("nRealRttUs is %f ms",NetStatus.nRealRttUs);
							//DEBUG("one second pass!");
							Current_Time = Tmp_Time;
							TmpCnt = SCnt;
						}
					}
					//获取此次连接传输的数据长度
					if(0 == First_Stream)
					{	
						count = read(events[idx].data.fd, buf, HLEN);
						First_Stream = 1;
						NetStatus.nFrameNum = ntohl(*((unsigned long *)(buf)));
						Buffer_Len = ntohs(*((unsigned short *)(buf + 8)));
						//DEBUG("NetStatus.nFrameNum is %d",NetStatus.nFrameNum);
						
						//DEBUG("Buffer_Len is %u",Buffer_Len);
						if((HLEN > Buffer_Len)||( MAXNUM < Buffer_Len))
						{
							DEBUG("len is too big!");
							break;
						}
						count = read(events[idx].data.fd, nbuf, Buffer_Len - HLEN);
						SCnt++;
					}
					else
					{
						count = read(events[idx].data.fd, nbuf, Buffer_Len);
						NetStatus.nFrameNum = ntohl(*(unsigned long *)(nbuf));
						SCnt++;
					}
					
					if(-1 == count)
					{
						if(errno != EAGAIN)
						{
							//DEBUG("read begin!");
							nready = 1;
						}
						TCnt++;
						usleep(100);
						if(TCnt >= 100)
						{
							TCnt = 0;
							break;	
						}
						
					}
					else if(0 == count)
					{
						nready = 1;
						break;
					}
					else
					{
						
					}
				}
				if(nready)
				{
					//DEBUG("Closed connection!");
					close(events[idx].data.fd);
				}
			}
		}
			
	}
	//以下代码段暂不使用
	free(events);
	close(socketHandle);
	RET_OK;
}



