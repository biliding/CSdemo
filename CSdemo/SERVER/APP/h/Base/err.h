/**
 * @file:   err.h
 * @note:   test
 * @brief:  错误码头文件
 * @author: dingjiayu
 * @date:   2021/12/7
 * @note:
 * @History:
   1.日    期:   2021/12/7
     作    者:   dingjiayu
     修改历史: 创建文件
 */
#ifndef __ERR_H__
#define __ERR_H__
/*----------------------------------------------*/
/*                 包含头文件                   */
/*----------------------------------------------*/

/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/
#define   ERR_COM_INVALID_PARAM      0x01     //无效参数
#define   ERR_COM_FAILED_INIT        0x02	  //socket初始化失败
#define   ERR_COM_TIME_OUT   		 0x03     //通讯超时
#define   ERR_COM_CONNECT_BREAK      0x04     //连接断开
#define   ERR_COM_EPOLL_INIT         0x05     //epoll初始化失败
#define   ERR_COM_EPOLL_CTRL         0x06     //epoll_ctrl失败

#endif

