#ifndef _SEARCH_MAIN_SVR_H_
#define _SEARCH_MAIN_SVR_H_

#include <fstream>
#include <string>

using namespace std;

/**
* @author feimat@baidu.com
*
* 
* <pre>
* ״̬��������ģ��
*
* </pre>
**/


#include "Define.h"





class CMainAsyncSvr
{
public:
    static void *run(void *instance);


	CMainAsyncSvr();
    virtual ~CMainAsyncSvr(){}

	void MainLoop();
    virtual xCallbackObj *CreateAsyncObj(CDataBuf *item); // ҵ��ʵ��

    CCallbackObjQueue mObjQueue;
	CMetaQueue<CDataBuf> mPending;

};


#endif 
