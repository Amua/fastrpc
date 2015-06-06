#ifndef _SEARCH_BACK_NET_SVR_H_
#define _SEARCH_BACK_NET_SVR_H_

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <string>
#include <map>
#include <vector>
#include <list>


#include "Define.h"
#include "ependingpool.h"
    

using namespace std;


/**
* @author feimat@baidu.com
*
* 
* <pre>
* ���Ӻ�̨�������첽����ģ��
*
* </pre>
**/






class CBackSocketReq
{
public:
    CRWCache sendcache;
    CRWCache recvcache;

    /*
    ���غ������ж��������ݽ����Ƿ�����

    is_complete �� check_recv_one ����ֵ
    ret<0 С����,����ʧ��
    ret>0 ������,���صõ����ֽ���
    ret=0 ������,��Ҫ��������
    // ע����������� CLIENT_COMPLETE_MAX_BUFFER �� BACK_COMPLETE_MAX_BUFFER����Ҫ��������
    */
    virtual int is_complete(char *data,unsigned data_len);

    // ����Ƿ��յ�һ�������İ���Ȼ������ŵ�buff���棬len�ǳ���
    int check_recv_one(char *buff,unsigned &len)
    {
        int ret = is_complete(recvcache.data(),recvcache.data_len());
        if ( ret>0 ) // �յ�һ�������İ�
        {
            memcpy(buff,recvcache.data(),ret);
            recvcache.skip(ret);
            len = ret;
            return ret;
        }
        return ret;
    }


    CBackSocketReq(){}
    virtual ~CBackSocketReq(){}

};


int read_back_svr(int sock, void **arg);
int write_back_svr(int sock, void **arg);
int init_back_svr(int sock, void **arg);
int clear_back_svr(int sock, void **arg);



/*
    Э�����ݽṹ
    ��nshead������̬���ݡ�û��ReqHeader
*/
class CBackNetSvr
{
public:
    static void *run(void *instance);

    // buf_len ���ջ������ĳ���
    CBackNetSvr(const int &_max,int _sock_num=10000,int _queue_len=10000);
    ~CBackNetSvr(){}

    void Start();
    void MainLoop();
  

    unsigned max_conn;
    CMetaQueue<CDataBuf> mPending;

	CIp2Handle ip2back_flow;

    ependingpool pool;
    int pool_sock_num;
    int pool_queue_len;

};


#endif
