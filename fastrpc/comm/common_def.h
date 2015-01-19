/**
************************************
* 	Copyright (c) 2012��Baidu
*	All rights reserved.
************************************
*\n
* 	@file common_def.h
* 	@breif ��ȫ�ƶ˷�����ȫ�ֶ���
*\n
* 	@version 0.0.1
* 	@author clarencelei
* 	@date 2012.3.12
*/
#ifndef _SRC_INCLUDE_COMMON_DEF_H_
#define _SRC_INCLUDE_COMMON_DEF_H_
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define DELETE_OBJ(x) \
{ \
if (NULL != x) \
{ \
	delete x;x=NULL;\
} \
}

const int DBNAME_LEN = 20;
const int HOST_LEN = 100;
const int USER_LEN = 20;
const int PASSWD_LEN = 30;
const int MAX_SLAVE_NUM = 10;  // ���slave����
const int MAX_REDIS_NUM = 5;   // ���֧�ֲ�ѯ��redis����ĸ���

const int SQL_LEN = 1024;  // SQL�����󳤶�
const int REDIS_CMD_LEN = SQL_LEN; // REDIS ������󳤶�
const int SINGLE_RESULT_LEN = SQL_LEN; // �������ؼǹ��ĳ���
const int MAX_RESULT_LEN = SQL_LEN;
const int MAX_USER_AGENT_LEN = SQL_LEN;

const int FILE_ID_LEN = 32;
const int MAX_IP_LEN = 20;
const int MAX_PROGRAM_INFO_LEN = 20;
const int MAX_MD5_NUM = 100;
const int MAX_MD5_LEN = FILE_ID_LEN * MAX_MD5_NUM;
const int MAX_RES_LEN = MAX_MD5_NUM * SINGLE_RESULT_LEN;

const int MAX_GUID_LEN = 100;
const char UNKOWN_GUID[] = ""; // �˴�����һ���ո�
const char UNKOWN_USER_AGENT[] = "";


// ��֤KEY���������ַ��� [0x20 0x7E]�У��������������ַ��� + �ո� / ? % # & =
const char XorKey[FILE_ID_LEN] = {
	0x01,0x02,0x00,0x01,0x02,0x00,0x01,0x02,
	0x01,0x03,0x01,0x02,0x02,0x01,0x02,0x03,
	0x02,0x02,0x02,0x00,0x03,0x02,0x00,0x03,
	0x03,0x01,0x03,0x01,0x02,0x03,0x01,0x02};

typedef enum EM_CODE_TYPE
{
	NOT_CODE = 0, // URL���
	BE_CODE = 1  // MD5���
};

typedef enum EM_CONN_TYPE
{
	WRITE_TYPE = 0, // д����
	READ_TYPE = 1   // ������
};

const int REDIS_FILE_HASH_KEY_LEN = 5 + FILE_ID_LEN;
const int REDIS_MGET_MAX = 20;
const char REDIS_MGET_CMD[] = "MGET %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s";

// ���������ѯ��key
typedef struct tagMultiKeys
{
	int nKeyNum;// key �ĸ���
	char strKeys[MAX_MD5_NUM][REDIS_FILE_HASH_KEY_LEN + 1]; // ���key������

	// �ṹ�幹�캯��
	tagMultiKeys()
	{
		nKeyNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strKeys[i][0] = '\0';
		}
	};

	// ��ʼ���ṹ��
	bool Init()
	{
		nKeyNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strKeys[i][0] = '\0';
		}
		return true;
	};

	void UnInit()
	{
		//
	};

	void Reset()
	{
		nKeyNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strKeys[i][0] = '\0';
		}
	};
}MULTI_KEYS;

// ���������ѯ�ķ���ֵ
typedef struct tagMultiValues
{
	int nValueNum;// value�ĸ���
	char strValues[MAX_MD5_NUM][SINGLE_RESULT_LEN + 1]; // ���value������

	// �ṹ�幹�캯��
	tagMultiValues()
	{
		nValueNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strValues[i][0] = '\0';
		}
	};

	// ��ʼ���ṹ��
	bool Init()
	{
		nValueNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strValues[i][0] = '\0';
		}
		return true;
	};

	void UnInit()
	{
		//
	};

	void Reset()
	{
		nValueNum = 0;
		for (int i = 0; i < MAX_MD5_NUM; ++i)
		{
			strValues[i][0] = '\0';
		}
	};
}MULTI_VALUES;

// ����ṹ��
typedef struct tagProtocolNode
{
	int nBeCode;						// �Ƿ����

	int nFileNum;					// ������ѯ��md5����

	char strFileMd5[MAX_MD5_LEN + 1];	// ����md5��ѯ��ԭʼ��

	char strFileId[MAX_MD5_NUM][FILE_ID_LEN + 1];  // ��ʽ��Ϊ��д���md5

	char strResult[MAX_RES_LEN + 1]; // ���ؽ��

	char strUserAgent[MAX_USER_AGENT_LEN + 1];

	char strGuid[MAX_GUID_LEN + 1];     // �ͻ��˵�GUID

	char strIp[MAX_IP_LEN + 1];         // �ͻ��˵�IP

	char strProgramName[MAX_PROGRAM_INFO_LEN + 1];              // �ͻ��˳�������

	char strProgramVer[MAX_PROGRAM_INFO_LEN + 1];				// �ͻ��˳���汾


	// ����ṹ�幹�캯��
	tagProtocolNode()
	{
		memset(this,0,sizeof(tagProtocolNode));
	};

	// ��ʼ���ṹ��
	bool Init()
	{
		memset(this,0,sizeof(tagProtocolNode));
		return true;
	};

	void UnInit()
	{

	};

	void Reset()
	{
		memset(this,0,sizeof(tagProtocolNode));
	};
}PROTOCOL_NODE;

// ������
const int OK = 1;
const int ERROR = -1;
const int NO_RESULT = 2;



#endif

