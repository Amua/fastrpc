/**
************************************ 
* 	Copyright (c) 2012��Baidu
*	All rights reserved.
************************************
*\n
* 	@file mylib.h
* 	@breif ���û��������Ķ���
*\n
* 	@version 0.0.1
* 	@author clarencelei
* 	@date 2012.3.12
*/
#ifndef _SRC_INCLUDE_MY_LIB_H_
#define _SRC_INCLUDE_MY_LIB_H_

#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>

namespace MyLib
{
	void InitDaemon();

	
	/*
	*	@brief �ַ�������
	*	@param[out] char *strDes Ҫ������Ŀ���ַ���
	*	@param[in] const char *strSrc Ҫ������Դ�ַ���
	*	@param[in] DWORD dwLen Ŀ���ַ�������
	*	@return char *����Ŀ���ַ���ָ��;
	*/
	char *SafeStrCopy(char *strDes, const char *strSrc, int nLen);


	// �������ַ���ת��Ϊ����
	int SafeAtoi(const char *str);


	int SafeStrCat(char* buf, const char* s1, const char* s2, const int &nBufLen);

	char* SafeStrUpr(char* str);

	bool StringIsAlnum(char* str);

	char *SafeAton(in_addr addr, char *str);
};

#endif
