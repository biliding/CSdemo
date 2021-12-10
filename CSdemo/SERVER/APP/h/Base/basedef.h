/**
 * @file:   basedef.h
 * @note:   test
 * @brief:  基础功能封装头文件
 * @author: dingjiayu
 * @date:   2021/12/7
 * @note:
 * @History:
   1.日    期:   2021/12/7
     作    者:   dingjiayu
     修改历史: 创建文件
 */
#ifndef __BASE_DEF_H__
#define __BASE_DEF_H__
/*----------------------------------------------*/
/*                 包含头文件                   */
/*----------------------------------------------*/
#include <cstdio>
/*----------------------------------------------*/
/*                 宏类型定义                   */
/*----------------------------------------------*/

#define DEBUG(format,...)      printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__) 

#define OK     			0
#define RET_OK 			{return OK;}
#define ERR             int								//返回的错误码

#endif
