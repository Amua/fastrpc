#ifndef _SVR_CNET_FUN_CLIENT_H_
#define _SVR_CNET_FUN_CLIENT_H_


#include <vector>
#include <errno.h>
#include <string>
#include <sys/time.h>
#include "CRWCache.h"


using namespace std;


/**
* @author feimat@baidu.com
*
*
* <pre>
* �����д����
* </pre>
**/




const unsigned CLIENT_COMPLETE_MAX_BUFFER = (30*1024*1024); // ���տͻ������İ�
const unsigned BACK_COMPLETE_MAX_BUFFER = (1);   // ���պ�̨���İ�



int do_accept(int sockfd);
int do_setsocktonoblock(int sd);
int do_tcplisten(const char *strip,int port, int queue);



string sock2peer(const int &sock);
int sock2peer(const int &sock,unsigned long &addr_4byte);
string ip2str(unsigned long addr);
int str2ip(const string &addr,unsigned long &addr_4byte);
int interaction_str2ip(const string &addr,unsigned long &addr_4byte);
int interaction_sock2ip(const int sock,unsigned long &addr_4byte);
char *time2str(time_t t,char *szDateTime);


unsigned GenClientFlowNo(int offset,int sock);
int GetOffsetFromFlowNo(unsigned flow,int &offset,int &sock);

long GetMillisecondTime();
long GetMicrosecondTime();

/*
read_size		�����Ѿ���ȡ�ĳ���
buff_size       buff�Ļ��峤��
return 0        �ɹ�
return -1       ʧ�ܣ���Ҫ�ر�socket
return -2       �ر�
*/
int noblock_read_buff(int sock, char *buff, unsigned *read_size, unsigned buff_size);

/*
buff				д������
buff_size			׼��д��Ĵ�С
write_succ_size     ��������д��Ĵ�С
return 0			�ɹ�
return -1			ʧ��
return -2       �ر�
*/
int noblock_write_buff(int sock, CRWCache& sendcache, unsigned *write_succ_size);







#endif

