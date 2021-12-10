/**
 * @file:   client.h
 * @note:   test
 * @brief:  �����ͷ�ļ�
 * @author: dingjiayu
 * @date:   2021/12/7
 * @note:
 * @History:
   1.��    ��:   2021/12/7
     ��    ��:   dingjiayu
     �޸���ʷ: �����ļ�
 */
#ifndef __CLIENT_H__
#define __CLIENT_H__
/*----------------------------------------------*/
/*                 ����ͷ�ļ�                   */
/*----------------------------------------------*/

#include "basedef.h"
#include "err.h"
#include <sys/time.h>
#include "string.h"
#include <stdlib.h>
#include <malloc.h>

/*----------------------------------------------*/
/*                 �����Ͷ���                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 ��������                     */
/*----------------------------------------------*/
/**
 * @function:   SocketClientInit
 * @brief:      Socket�ͻ��˳�ʼ��
 * @param[in]:  char *pIpAddr			IP��ַ�ַ���
 * @param[in]:  unsigned int port		�˿ں�
 * @param[in]:  char *pEthDev			���������ַ�
 * @param[in]:  unsigned int timeout	���ӳ�ʱʱ��
 * @param[out]: int *pSocketHandle     socket���
 * @return:     ERR                     ������
 */
 ERR SocketClientInit(char *pIpAddr, unsigned int port, char *pEthDev, unsigned int timeout, int *pSoctetHandle);
 
 
  /**
 * @function:   SocketSend
 * @brief:      socket��������
 * @param[in]: 	int socketHandle     		socket���
 * @param[in]: 	int *pBuffer     	   		���ݻ���
 * @param[in]: 	unsigned int len       		���ݳ���
 * @param[out]: 	NONE
 * @return:     ERR                     	������
 */
ERR SocketSend(int socketHandle, char *pBuffer, unsigned int len);

 /**
 * @function:   SocketClose
 * @brief:      �ر�socket
 * @param[in]: 	int socketHandle     		socket���
 * @param[out]: 	NONE
 * @return:     ERR                     	������
 */
ERR SocketClose(int socketHandle);

/**
 * @function:   EnPacket
 * @brief:      ���з��
 * @param[in/out]: 	char *SendBuf     		��Ҫ���з�װ��buffer
 * @param[int]: 	int Send_Len			���ݳ���
 * @param[int/out]: 	int firstFlag		�Ƿ��ǵ�һ֡
 * @return:     ERR							������
 */
ERR EnPacket(char *sendBuf, int send_Len, int *firstFlag);

#endif