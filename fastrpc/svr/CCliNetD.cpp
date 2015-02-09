#include <iostream>
#include <assert.h>
#include <sys/time.h>

#include "CCliNetD.h"
#include "CSvr.h"
#include "CConnectPoll.h"
#include "Statistic.h"


using namespace std;

void DebugSockInfo(const char *info,int sock,int ret,unsigned rws=10240)
{
    return;
    string strIp = sock2peer(sock);

    if (rws==10240) LOG(LOG_ALL,"%s|sock|%d|%s|errno|%d|%s|ret|%d\n",strIp.c_str(),sock,info,errno,strerror(errno),ret);
    else LOG(LOG_ALL,"%s|sock|%d|%s|errno|%d|%s|ret|%d|rw_size|%u\n",strIp.c_str(),sock,info,errno,strerror(errno),ret,rws);
}


void DebugSockInfo2(const char *info,int sock,int ret,unsigned rws,unsigned dlen)
{
    return;
    string strIp = sock2peer(sock);

    LOG(LOG_ALL,"BBB|%s|sock|%d|%s|errno|%d|%s|ret|%d|rw_size|%u|data_len|%u\n",
        strIp.c_str(),sock,info,errno,strerror(errno),ret,rws,dlen);
}


unsigned CliDFlowNo(unsigned& cliflow) {
    cliflow = ((cliflow == 0 || cliflow == 0xffffffffUL ) ? 1 : cliflow+1);
    return cliflow;
}

CClientSocketReq::CClientSocketReq(){
    sendcache.SetLock(true);
    keep_live = true;
    gettimeofday(&tBegin,NULL);
}

CClientSocketReq::~CClientSocketReq(){
    gettimeofday(&tEnd,NULL);//STAT_ADD("SocketDue",0,&tBegin,&tEnd,1);
}



/*
* <0 写失败，会触发SOCK_CLEAR事件
* =0 数据全部写完毕，并将sock重新放入epoll中进行监听读请求(相当于长连接)
* =1 数据写成功，但没有全部写完，放回epoll等待下次再监听到可写状态

*/

int write_client(ependingpool* ep, int sock, void **arg)
{
    unsigned write_succ_size = 0;
	int ret = -1;

	// 可写，直接从缓存在发送
	CClientSocketReq *psock_req = (CClientSocketReq*)(*arg);


    unsigned haswrite=0;
    //unsigned i = 0;
    //while ( i<10000 ) {
    while (true ) {
        psock_req->sendcache.extern_lock();

        if ( psock_req->sendcache.data_len() == 0 ) {
            int w_ret = -1;
            if ( psock_req->keep_live ) {
                ep->reset_item(psock_req->offset, true);
                w_ret = 1;
            }
            psock_req->sendcache.extern_unlock();
            return w_ret;
        }
        ret = noblock_write_buff(sock,psock_req->sendcache,&write_succ_size);
        if ( ret!=0 ) {
            if(ret == -1) {
                LOG(LOG_ALL,"write buf failed.ip is %s and data len is %s\n",sock2peer(sock).c_str(),psock_req->sendcache.data_len());
            }
            if(ret == -2) {
                LOG(LOG_ALL,"client closed detected by write.ip is %s and data len is %s\n",sock2peer(sock).c_str(),psock_req->sendcache.data_len());
            }
            psock_req->sendcache.extern_unlock();
            return -1;
        }
        if ( write_succ_size>0 ) psock_req->sendcache.skip(write_succ_size);

        psock_req->sendcache.extern_unlock();

        if ( ret==0 && errno==EAGAIN && write_succ_size==0 ) {
            break;
        }

    }


    psock_req->sendcache.extern_lock();
    if ( psock_req->sendcache.data_len() == 0 ) {
        int w_ret = -1;
        if ( psock_req->keep_live ) {
            ep->reset_item(psock_req->offset, true);
            w_ret = 1;
        }
        psock_req->sendcache.extern_unlock();
        return w_ret;
    }
    psock_req->sendcache.extern_unlock();
    return 1;
}
/*
* <0 读失败，会触发SOCK_CLEAR事件
* =0 数据全部读取完毕，并将sock放入就绪队列中
* =1 数据读成功，但没有全部读取完毕，需要再次进行监听，sock会被放回epoll中
* =2 放到todo event

*/
int read_client(ependingpool* ep, int sock, void **arg)
{
	int ret = 0;
	unsigned read_size = 0;
    CClientSocketReq *psock_req = (CClientSocketReq*)(*arg);
    ShareParam* sp = (ShareParam*)ep->ext_data;
    char*& client_recv_buff = sp->client_recv_buff;
    unsigned& client_recv_len = sp->client_recv_len;

    unsigned hasread=0;
    //unsigned i = 0;
    //while ( i<10000 ) {
    while ( true ) {
        ret = noblock_read_buff(sock,client_recv_buff,&read_size,client_recv_len);
        if ( ret!=0 ){
            if( ret == -1)
            {
                 LOG(LOG_ALL,"read buf failed.ip is %s and data is %s\n",sock2peer(sock).c_str(),client_recv_buff);
            }
            if( ret == -2)
            {
                 //LOG(LOG_ALL,"client closed detected by read. ip is %s and data is %s\n",sock2peer(sock).c_str(),client_recv_buff);
            }

            return -1;    }
        if ( read_size>0 ) psock_req->recvcache.append(client_recv_buff,(unsigned)read_size);

        if ( read_size>0 ) hasread += read_size;
        if ( ret==0 && errno==EAGAIN && read_size==0 ) {
            //LOG(LOG_ALL,"EAGAIN|sock %d|i|%u|%d|%s|has read|%u\n",sock,i,errno,strerror(errno),hasread);
           // i=2000000;
            //continue;
            break;
        }
        //LOG(LOG_ALL,"INFO|sock %d|i|%u|%d|%s|has read|%u\n",sock,i,errno,strerror(errno),hasread);
      //  i++;
    }


    // 直接返回2，todo event操作
    return 2;
}

int todo_client(ependingpool* ep, struct ependingpool::ependingpool_task_t *v)
{
    int sock = v->sock;
    int offset = v->offset;
    void *arg = v->arg;


    CClientSocketReq *psock_req = (CClientSocketReq *)(arg);
    ShareParam* sp = (ShareParam*)ep->ext_data;
    char* client_recv_buff = sp->client_recv_buff;
    int client_recv_len = sp->client_recv_len;
    unsigned one_len = 0;
    int ret = 0;

    CASyncSvr* p_svr = sp->p_svr;

    do {
        ret = psock_req->check_recv_one(client_recv_buff,one_len);
        if ( ret>0 ) {
            unsigned long ip = 0;
            sock2peer(sock,ip);

            // 产生flow_no,必须保存offset
            unsigned cd_flow = psock_req->flow;
            psock_req->offset = offset;
            sp->cliF2O->Add(cd_flow,offset);

            if ( p_svr->IsAsyncSvr() ) CliD_MainD(p_svr,cd_flow,psock_req->recvcache.data(),one_len,ip,0);
            else CliD_Pool(p_svr,cd_flow,psock_req->recvcache.data(),one_len,ip,0);
            psock_req->recvcache.skip(one_len);

            if ( psock_req->recvcache.data_len()==0 ) ret = 0;
        }
    } while( ret>0 );

    // 返回0，监听 OUT，写操作
    // 返回1，监听 IN
    // 返回2，关闭连接clear_item
    // 返回3，什么也不做
    if ( ret >= 0 ) {
		// 这个包还没接收完，需要继续读
        return 3;
	}
	// 读失败，触发SOCK_CLEAR事件
	return -1;
}

int ini_client(ependingpool* ep, int sock, void **arg)
{
	CClientSocketReq *sock_req = new CClientSocketReq();
    ShareParam* sp = (ShareParam*)ep->ext_data;
    sock_req->flow = CliDFlowNo(sp->flow);
	if (sock_req == NULL)
	{
		return -1;
	}
	//将指针赋给sock捆绑的指针
	*arg = sock_req;
	return 0;
}

int clear_client(ependingpool* ep, int sock, void **arg)
{
	CClientSocketReq *req = (CClientSocketReq*)(*arg);
    ShareParam* sp = (ShareParam*)ep->ext_data;
    CASyncSvr* p_svr = sp->p_svr;
	if ( req!=NULL ){
        if (p_svr->_close_handler) {
            p_svr->_close_handler(p_svr,req->flow,p_svr->_close_handler_param);
        }
        sp->cliF2O->Del(req->flow);
		delete req;
		req = NULL;

        string strip = sock2peer(sock);
        //LOG(LOG_ALL,"CLI|%s close|sock:%d\n",strip.c_str(),sock);
	}
	return 0;
}


int accept_client(ependingpool* ep, int lis, void **arg)
{
    int work_sock;
    work_sock = do_accept(lis);

    if ( work_sock == -1 ) {
        //LOG(LOG_ALL,"do_accept(%d) call failed.error[%d] info is %s.", work_sock,errno, strerror(errno));
        return -1;
    }

    DebugSockInfo("accept client 1",work_sock,0);

    //这里需要设置非堵塞模式， 否则读写的时候会被hand住
    if (do_setsocktonoblock(work_sock)) {
        close(work_sock);

        DebugSockInfo("accept client 2 block fail",work_sock,0);
        LOG(LOG_ALL,"do_setsocktonoblock failed.sock[%d] error[%d] info is %s.",work_sock,errno,strerror(errno));
        return -1;
    }
    return work_sock;

}

int server_hup(ependingpool* ep, int sock,void **arg){
     //LOG(LOG_ALL,"sock error: %d server socket error detected by epollhup.\n", sock);
     return 0;

}
int sock_error(ependingpool* ep, int sock,void **arg){
    LOG(LOG_ALL,"sock error: %d server socket error detected by epollerror.\n", sock);
    return 0;
}
int sock_close(ependingpool* ep, int sock,void **arg){
    //LOG(LOG_ALL,"sock will be closed.\n", sock);
    return 0;
}

CClientNetSvr::CClientNetSvr(CASyncSvr* asvr,const int &_max,int _sock_num/* =10000 */,int _queue_len/* =10000 */)

{
    p_svr = asvr;
    max_conn = _max;

    pool_sock_num = _sock_num;
    pool_queue_len = _queue_len;

    pool.set_epoll_timeo(-1);
    //设置连接超时时间(秒), 默认为1s
    pool.set_conn_timeo(60*30);
    //设置读超时时间(秒), 默认为1s
    pool.set_read_timeo(60*30);
    //设置写超时时间(秒), 默认为1s
    pool.set_write_timeo(60*30);

    //设置可存储socket的数量
    pool.set_sock_num(max_conn);
    //设置已就绪队列的长度
    pool.set_queue_len(pool_sock_num);
    sp = new ShareParam(CLIENT_COMPLETE_MAX_BUFFER);
    sp->p_svr = p_svr;
    pool.ext_data = sp;
}

CClientNetSvr::~CClientNetSvr() {
    delete sp;
}

int read_time_out(ependingpool* ep, int sock, void **arg) {
    LOG(LOG_ALL,"sock error: %d read time out.\n", sock);
    return 0;
}

int write_time_out(ependingpool* ep, int sock, void **arg) {
    LOG(LOG_ALL,"sock error: %d write time out.\n", sock);
    return 0;
}

int listen_time_out(ependingpool* ep, int sock, void **arg) {
    LOG(LOG_ALL,"sock error: %d listen time out.\n", sock);
    return 0;
}

void CClientNetSvr::Start(int listen_sd)
{

    int ret;
    //printf("listen %d,%s\n",listen_sd,strIP.c_str());

    pool.set_listen_fd(listen_sd);
    //针对不同的事件使用相应的回调函数
    ret = pool.set_event_callback(ependingpool::SOCK_ACCEPT, accept_client);   //printf("set ret%d\n",ret);


    //初始化与sock捆绑的数据
    pool.set_event_callback(ependingpool::SOCK_INIT, ini_client);
    //释放与sock捆绑的数据
    pool.set_event_callback(ependingpool::SOCK_CLEAR, clear_client);
    //使用非堵塞读写方式，模拟异步读写
    pool.set_event_callback(ependingpool::SOCK_READ, read_client);
    pool.set_event_callback(ependingpool::SOCK_WRITE, write_client);
    pool.set_todo_event_ex(todo_client, NULL);
   // set timeout callback,by liuyulian; 2014-04-08
    pool.set_event_callback(ependingpool::SOCK_READTIMEOUT, read_time_out);
    pool.set_event_callback(ependingpool::SOCK_WRITETIMEOUT, write_time_out);
    pool.set_event_callback(ependingpool::SOCK_LISTENTIMEOUT, listen_time_out);
    pool.set_event_callback(ependingpool::SOCK_HUP,server_hup);
    pool.set_event_callback(ependingpool::SOCK_CLOSE,sock_close);
    pool.set_event_callback(ependingpool::SOCK_ERROR,sock_error);

    return;
}

void CClientNetSvr::MainLoop()
{
    int ret = 0;

    while (pool.is_run())
    {
        pool.check_item();
    }// while
    return;
}

void *CClientNetSvr::run(void *instance)
{
    CClientNetSvr *client_svr = (CClientNetSvr*)instance;
    pthread_t tid = pthread_self();
    LOG(LOG_ALL,"Info:IndexD CClientNetSvr detach thread create,pid=%d,tid=%u\n",getpid(),tid);

    //tid = 0;
    //int ret = pthread_create(&tid,NULL,QUEUELoop,instance);
    //assert(ret==0);

    client_svr->MainLoop();

    return NULL;
}

