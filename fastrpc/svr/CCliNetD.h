#ifndef _SEARCH_CLIENT_NET_SVR_H_
#define _SEARCH_CLIENT_NET_SVR_H_

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
* ǰ�˽����첽���߳�ģ��
*
* </pre>
**/

class CClientSocketReq
{
public:
    bool keep_live;
    CRWCache sendcache;
    CRWCache recvcache;

    /*
    ���غ������ж��������ݽ����Ƿ�����

    is_complete �� check_recv_one ����ֵ
    ret<0 С����,����ʧ��
    ret>0 ������,���صõ����ֽ���
    ret=0 ������,��Ҫ��������
    ע����������� CLIENT_COMPLETE_MAX_BUFFER �� BACK_COMPLETE_MAX_BUFFER����Ҫ��������
    */
    virtual int is_complete(char *data,unsigned data_len);

    // ����Ƿ��յ�һ�������İ���Ȼ������ŵ�buff���棬len�ǳ���
    int check_recv_one(char *buff,unsigned &len)
    {
        len = 0;
        int ret = is_complete(recvcache.data(),recvcache.data_len());
        if ( ret>0 ) // �յ�һ�������İ�
        {
            //memcpy(buff,recvcache.data(),ret);
            //recvcache.skip(ret);

            //if ( recvcache.data()==NULL ) LOG(LOG_ALL,"NULLxxxxxxx ret %d,%u\n",ret,recvcache.data_len());
            //LOG(LOG_ALL,"xxxxxxx ret %d,%u\n",ret,recvcache.data_len());

            len = ret;
            return ret;
        }
        return ret;
    }

    CClientSocketReq();
    virtual ~CClientSocketReq();

    unsigned flow;
    int offset;
    // ͳ��
    timeval tBegin;
    timeval tEnd;
};

//int todo_client(struct ependingpool::ependingpool_task_t *v);
//int read_client(int sock, void **arg);
//int write_client(int sock, void **arg);
//int accept_client(int lis, void **arg);
//int ini_client(int sock, void **arg);
//int clear_client(int sock, void **arg);
//int  server_hup(int sock,void **arg);
//int read_time_out(int sock,void **arg);
//int write_time_out(int sock,void **arg);
//int listen_time_out(int sock,void **arg);
//int sock_close(int sock,void **arg);
//int sock_error(int sock,void **arg);


/*
    Э�����ݽṹ
    ��nshead������̬���ݡ�û��ReqHeader
*/
typedef struct _ShareParam {
    _ShareParam (int max_len) {
        client_recv_len = max_len;
        client_recv_buff = new char[max_len];
        cliF2O = new CFlow2Offset();
        flow = 0;
    }
    ~_ShareParam () {
        if (client_recv_buff) {
            delete []client_recv_buff;
            delete cliF2O;
        }
    }
    char* client_recv_buff;
    unsigned client_recv_len;
    CFlow2Offset* cliF2O;
    unsigned flow;
    CASyncSvr* p_svr;
} ShareParam;

class CClientNetSvr
{
public:
    static void *run(void *instance);

    // buf_len ���ջ������ĳ���
    CClientNetSvr(CASyncSvr* asvr,const int &_max,int _sock_num=10000,int _queue_len=10000);
    ~CClientNetSvr();

    void Start(int listen_sd);
    void MainLoop();


    unsigned max_conn;
    CMetaQueue<CDataBuf> mPending;

    ependingpool pool;
    int pool_sock_num;
    int pool_queue_len;
    int pool_listen_fd;

    ShareParam* sp;
    CASyncSvr* p_svr;

};





/*

=2 �����ɹ��� ����SOCK_CLEAR�¼��������رվ�������԰Ѿ���Ƴ�ependingpool


_block_size�������Ѿ�����Ŀռ�

connect �� status_send_connecting


dcc_req_send:  // һ������ֻ�õ������Ȼ��ֱ�Ӿ��� status_send_connecting

(ret != -EWOULDBLOCK) && (ret != -EINPROGRESS) ������connect���������������´ξͿɶ���д

epoll������ģʽ,Edge Triggered(���ET) �� Level Triggered(���LT).�ڲ���������ģʽʱҪע�����,�������ETģʽ,��ô����״̬�����仯ʱ�Ż�֪ͨ,������LTģʽ������ԭ����select/poll����,ֻҪ����û�д�����¼��ͻ�һֱ֪ͨ.

�߽紥����Ч�ʸߣ�������ʵ����ҪС�ģ�©��û����Ĳ���õ������ˡ�
����ƽ�����ĳ���ʵ���Ϸ��㣬Ч��Ҫ��Щ���ƺ�libevent��lighttpd���õ�����


EPOLLIN  �ɶ�������socket�����ر�
EPOLLOUT ��д
EPOLLERR ��������
EPOLLHUP socket����
EPOLLONEAHOT ֻ����һ���¼�����������������ٴ�epoll_ctl



keep_alive close(fd)������epoll_ctl(del)
reset_item(int handle, bool keep_alive)
pool_epoll_offset_mod(handle, EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLONESHOT);

clear_item
��close��fd����ֻ��pool_epoll_offset_del(handle);


����epoll������Ĭ�ϲ���
accept_sock
insert_item_sock



��CCD��

1. ��epoll event�� send ������ϣ���Ϊ IN,δ�����꣬�����κδ���
2. ��epoll event�� recv �����κδ���
3. ��item data�� send��ɣ������κδ���δ��ɣ���Ϊin��out



1.  ��item data���Ѿ����ӣ�send�����꣬�����κδ���δ�����꣬�޸�Ϊin/out
���������ӣ������꣬��ΪIN,δ�����꣬��Ϊin/out


2.  ��epoll event�� �����꣬��ΪIN��δ�����꣬�����κδ���
3.  ��epoll event�� recv �����κδ���



insert_item ���޸�Ϊ last_active��������Ϊ sock_status = READY
reset_item ������ֳ����� true ����last_active��������Ϊ sock_status = READY
write_reset_item ��last_active��������Ϊ sock_status = WRITE_BUSY

do_read_event
if (READ_BUSY != m_ay_sock[offset].sock_status) {

m_ay_sock[offset].last_active = time(NULL);

m_ay_sock[offset].sock_status = READ_BUSY;

}

do_write_event
if (WRITE_BUSY != m_ay_sock[offset].sock_status) {

m_ay_sock[offset].last_active = time(NULL);

m_ay_sock[offset].sock_status = WRITE_BUSY;

}

�ŵ��������У� ���޸�Ϊ last_active
m_ay_sock[offset].sock_status = BUSY;
*/

#endif
