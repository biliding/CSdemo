/**
 * @file:   main.c
 * @note:   test
 * @brief:  主函数入口文件
 * @author: dingjiayu
 * @date:   2021/12/8
 * @note:
 * @History:
   1.日    期:   2021/12/8
     作    者:   dingjiayu
     修改历史: 创建文件
 */

/*----------------------------------------------*/
/*                 包含头文件                   */
/*----------------------------------------------*/

#include "client.h"


/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/
#define IP         "10.1.74.232"
#define ETH0       "eth0"
#define PORT       9000
#define CNUM       3
#define SENDLEN    1024
/*----------------------------------------------*/
/*                 结构体定义                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 函数声明                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 全局变量                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 函数定义                     */
/*----------------------------------------------*/
/**
 * @function:   Time_Current
 * @brief:      程序主函数
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
 * @function:   main
 * @brief:      程序主函数
 * @return:     int
 * 注释：argv[1]为轮询的时间间隔，argv[2]为单次发送的数据量
**/
 int main(int argc, char **argv)
{
	int ret = 0;
	//通信句柄
	int pHandle[CNUM];
	unsigned long Current_Time = 0;
	unsigned long Tmp_Time;
	//client轮询的时间间隔，单位为S
	unsigned int Time_Gap = atoi(argv[1]);
	//client单次发送的数据量
	unsigned int Send_Len = atoi(argv[2]);
	int FirstFlag = 1;
	DEBUG("Gap is %d", Time_Gap);
	//入参检查，其中发送长度暂定10~1024之间，后续可调整
	if((0 > Time_Gap)||(10 > Send_Len)||(1024 < Send_Len))
	{
		DEBUG("Please enter right num!");
		return 0;
	}
	char *SendBuf = (char *)malloc(Send_Len);
	if(NULL == SendBuf)
	{
		DEBUG("malloc error!");
		return 0;
	}
	memset(SendBuf, 0, Send_Len);
	DEBUG("long size is %d", sizeof(unsigned long));
	Send_Len = Send_Len > SENDLEN ? SENDLEN : Send_Len;
	DEBUG("sendlen is %d",Send_Len);
	do
	{
		for(int i = 0; i < CNUM; i++)
		{
			//DEBUG("i is %d",i);
			Current_Time = Time_Current();
			ret = SocketClientInit(IP, PORT, ETH0, 0, &pHandle[i]);
			if(0 != ret)
			{
				DEBUG("Socket init err!");
				break;
			}
			FirstFlag = 1;
			
			while(1)
			{	
				ret = EnPacket(SendBuf, Send_Len, &FirstFlag);
				if(0 != ret)
				{
					DEBUG("EnPacket error!");
					break;
				}
				SocketSend(pHandle[i], SendBuf, Send_Len);
				Tmp_Time = Time_Current();
				//DEBUG("time is %ul",Tmp_Time);
				if(Tmp_Time >= Current_Time)
				{
					if((Tmp_Time - Current_Time) >= Time_Gap * 1000000)
					{
						//DEBUG("Time out");
						break;
					}
				}
				else
				{
					if((Tmp_Time + 0xFFFFFFFF^(~Current_Time)) >= Time_Gap * 1000000)
					{
						break;
					}
				}
				
			}
			SocketClose(pHandle[i]);
		}
		if(0 != ret)
		{
			DEBUG("ret error");
			break;
		}
		//DEBUG("data over");
	}while(1);
	return 0;
}

