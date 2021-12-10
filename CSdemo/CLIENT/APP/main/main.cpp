/**
 * @file:   main.c
 * @note:   test
 * @brief:  ����������ļ�
 * @author: dingjiayu
 * @date:   2021/12/8
 * @note:
 * @History:
   1.��    ��:   2021/12/8
     ��    ��:   dingjiayu
     �޸���ʷ: �����ļ�
 */

/*----------------------------------------------*/
/*                 ����ͷ�ļ�                   */
/*----------------------------------------------*/

#include "client.h"


/*----------------------------------------------*/
/*                 �����Ͷ���                   */
/*----------------------------------------------*/
#define IP         "10.1.74.232"
#define ETH0       "eth0"
#define PORT       9000
#define CNUM       3
#define SENDLEN    1024
/*----------------------------------------------*/
/*                 �ṹ�嶨��                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 ��������                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 ȫ�ֱ���                     */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 ��������                     */
/*----------------------------------------------*/
/**
 * @function:   Time_Current
 * @brief:      ����������
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
 * @brief:      ����������
 * @return:     int
 * ע�ͣ�argv[1]Ϊ��ѯ��ʱ������argv[2]Ϊ���η��͵�������
**/
 int main(int argc, char **argv)
{
	int ret = 0;
	//ͨ�ž��
	int pHandle[CNUM];
	unsigned long Current_Time = 0;
	unsigned long Tmp_Time;
	//client��ѯ��ʱ��������λΪS
	unsigned int Time_Gap = atoi(argv[1]);
	//client���η��͵�������
	unsigned int Send_Len = atoi(argv[2]);
	int FirstFlag = 1;
	DEBUG("Gap is %d", Time_Gap);
	//��μ�飬���з��ͳ����ݶ�10~1024֮�䣬�����ɵ���
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

