#include "my_lib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void MyLib::InitDaemon()
{
	int nPid;
	if(nPid = fork())
	{
		exit(0);// �Ǹ����̣�����������
	}
	else if(nPid< 0)
	{
		exit(1);//forkʧ�ܣ��˳�
	}

	// ��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤
	setsid();

	if(nPid=fork())
	{
		exit(0);// �ǵ�һ�ӽ��̣�������һ�ӽ���
	}
	else if(nPid< 0)
	{
		exit(1);
	}

	// �ǵڶ��ӽ��̣�����
	int nFdMax = getdtablesize();

	// �رմ򿪵��ļ�������,����0��1��2
	for(int i = 3;i < nFdMax; ++i)
	{
		close(i);
	}

	//chdir("/home/work");// �ı乤��Ŀ¼��
	umask(0);
	return;

}

char *MyLib::SafeStrCopy(char *strDes, const char *strSrc, int nLen)
{
	if(strDes == NULL || strSrc == NULL || nLen <= 1)
	{
		return strDes;
	}

	strncpy(strDes, strSrc, nLen);
	//ȷ����0�ַ�����β
	strDes[nLen - 1] = '\0';

	return strDes;
}


int MyLib::SafeAtoi(const char *str)
{
	if(str == NULL)
	{
		return 0;
	}

	return atoi(str);
}


int MyLib::SafeStrCat(char* buf, const char* s1, const char* s2, const int &nBufLen)
{
	int nS1Len = strlen(s1);
	int nS2Len = strlen(s2);
	if (nS1Len + nS2Len >= nBufLen) {
		return -1;
	}
	memmove(buf, s1, nS1Len);
	memmove(buf + nS1Len, s2, nS2Len);
	buf[nS1Len + nS2Len] = 0;
	return (int) (nS1Len + nS2Len);
}


char* MyLib::SafeStrUpr(char* str)
{
	if (NULL == str)
	{
		return str;
	}

	char* p = str;
	while ('\0' != *p)
	{
		if (*p >= 'a' && *p <= 'z')
			*p -= 0x20;
		p++;
	}

	return str;
}

bool MyLib::StringIsAlnum(char* str)
{
	if (NULL == str)
	{
		return false;
	}

	char* p = str;
	while ('\0' != *p)
	{
		if (!isalnum(*p))
		{
			return false;
		}
		p++;
	}

	return true;
}


char *MyLib::SafeAton(in_addr addr, char *str)
{
	unsigned int s = addr.s_addr;

	sprintf(str, "%d.%d.%d.%d",
		s & 0xff, (s >> 8) & 0xff, (s >> 16) & 0xff, (s >> 24) & 0xff);
	return str;
}

