#ifndef _SEARCH_THREAD_SVR_H_
#define _SEARCH_THREAD_SVR_H_

#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <semaphore.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>


#include "Define.h"


/**
*
*
* <pre>
* �첽���svr���������߳��첽svr���̳߳�ͬ��svr
* �첽��̰���3��ģ��
* ClientD ����Ϳͻ��˵����罻��
* MainD   ���첽�߼��Ĺ���
* BackD   ����ͺ�̨�����罻��
*
* ͬ�����svr,����2��ģ��
* ClientD ����Ϳͻ��˵����罻������������������ٽ���Pool
* Pool    ͬ���̴߳�����



* ���ӣ������뿴 CompleteFun.h ��˵����

* async_echo_svr �첽ģʽʵ�֣�client�������ݹ�����Ȼ��ֱ�ӷ���
* async_web_svr  �첽ģʽ��web�ϴ����ط�����ʵ��
* pool_web_svr   ͬ��ģʽ��web�ϴ����ط�����ʵ��
* </pre>
**/



using namespace std;


class CTask;
class CSearchTheadPool;
class CClientNetSvr;
class CBackNetSvr;
class CMainAsyncSvr;

/*
	�첽svr
	ʵ���첽ģʽ��Ҫ�Ĳ��裺
	1. ʵ�� int CClientSocketReq::is_complete(char *data,unsigned data_len)
	2. ʵ�� int CBackSocketReq::is_complete(char *data,unsigned data_len)
	3. ʵ�� xCallbackObj *CMainAsyncSvr::CreateAsyncObj(CDataBuf *item)
	�뿴 async_echo_svr ���� �� async_web_svr ����

	ͬ��ģʽ���첽ģʽ���л����ǳ��򵥣�async_web_svr �� pool_web_svr ���һ����

*/
typedef int ext_handler(CASyncSvr* svr, unsigned cli_flow, void* param);

class CASyncSvr {
public:
    CASyncSvr(const char *strip,int port,int max_conn,int threadnum,bool basync,unsigned svr_id);
    virtual ~CASyncSvr() {}

    int Start(int listen_sd);
    int Stop();
    bool IsAsyncSvr() { return _basync_svr; }

	// ��һ����Ҫ�����������ڳ�ʼ��ҵ�����
    virtual void Initail() { }
    virtual void Finish() {}

    // ͬ��ģʽ������ҵ��Task
    virtual CTask* CCreatePoolTask();

    void SendBack(unsigned clid_flow,char *data,unsigned len,unsigned _op = SEND_NORMAL);

    bool RegiCloseHandler(ext_handler* close_handler, void* close_handler_param);

public:
    string _svr_ip;
    int _svr_port;
    int _max_conn;
    int _thread_num;
    bool _basync_svr;
    unsigned _svr_id;

    CMetaThread _clientWorker;
    CClientNetSvr *_client_d;

    CMetaThread _mainWorker;
    CMainAsyncSvr *_main_d;

    CMetaThread _backWorker;
    CBackNetSvr *_back_d;

    CSearchTheadPool *_pool;
    void* rpc_param;
    ext_handler* _close_handler;
    void* _close_handler_param;
};

/*
	ͬ��ģʽ
	������ֻ��Ҫ�̳У���ʵ��Run������Initial��Finish���ڴ������ͷ�˽�б���
	����һ��svr�Ĳ��裺
	1. ʵ�� CClientSocketReq::is_complete
	2. CTask *CASyncSvr::CCreatePoolTask()
	3. �̳��� CTask����ʵ��Run����
	�뿴 pool_web_svr����ʵ��һ��web svrʵ���ϴ�����
*/



class CTask
{
public:
	virtual int Initial(CASyncSvr* svr) = 0;
	virtual void* Run(CDataBuf *item, CASyncSvr* svr) = 0;
    virtual int Finish(CASyncSvr* svr) = 0;

    CTask(){}
    virtual ~CTask(){}
};

class CSearchTheadPool
{
public:

    CSearchTheadPool(CASyncSvr* asvr,int threadNum);
    ~CSearchTheadPool() {
        sem_destroy(&_sem);
    }

    unsigned GetPendingQueue() {return m_queue_size;}
    int ThreadInfo(string &info);
    // ���̵߳��õĺ���
    int Start();
    int Stop();

    CDataBuf PopDataBuf();
    int PushDataBuf(unsigned flow,char *data,unsigned len, unsigned ip, unsigned short port);

    static void *ThreadProcess(void *argument);
    int SignStopStatus();
    bool PoolEnable();
    CASyncSvr* p_svr;
    int m_iStop;

protected:


    int BroadCast();

    int MoveToIdle(pthread_t tid);
    int MoveToBusy(pthread_t tid);

private:

	int m_iThreadNum;
    int m_queue_size;
	list<CDataBuf> clientQueue;

    sem_t _sem;
    CMutex _sem_mutex;
	CMutex _mutex;

	list<pthread_t> m_IdleThreadList;
	list<pthread_t> m_BusyThreadList;

};



// kill -USR1 18864
void InitDaemon();
void SignalAll();


extern int app_main(std::string& ip, unsigned short port, int sock_num,
                    int io_thread_num, int worker_num, void* rpc_param=NULL);

extern bool IsClientAsyn;
extern std::string Multi_Process_or_Thread;

#endif
