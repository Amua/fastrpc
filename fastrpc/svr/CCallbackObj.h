#ifndef _SVR_CCALLBACK_H_
#define _SVR_CCALLBACK_H_

#include <vector>
#include <errno.h>
#include <string>
#include <sys/time.h>



using namespace std;


/**
* @author feimat@baidu.com
*
* 
* <pre>
* ���ģ�ͣ��첽�ص�����
* </pre>
**/




enum {
    HANDLE_CODE_TIME_OUT = 100,
    HANDLE_CODE_FORCE_FINISH = 101,

    TIMEOUT_FORCE_RSP = 102,
};

unsigned FlowNo();
unsigned WhatTime();

class ObjectMethodBase 
{  
public:
    virtual ~ObjectMethodBase() {}
    virtual int execute(int code,void *data) = 0;
}; 

template<class T>
class ObjectMethod : public ObjectMethodBase 
{
public:
    typedef int (T::*MethodPtr)(int code, void *data);
    ObjectMethod(T* optr,MethodPtr mptr) : mOptr(optr), mMptr(mptr) {}

    int execute(int code,void *data) 
    {
        return (mOptr->*mMptr)(code,data);
    }

private:
    T*  mOptr;      // pointer to the object
    MethodPtr   mMptr;	// pointer to the method
};

template<class T> 
void SET_HANDLER( T* pobj, typename ObjectMethod<T>::MethodPtr meth )
{
    pobj->SetCallback( new ObjectMethod<T>(pobj, meth) );
}

struct CStepInfo {
    unsigned nowTime;
    int step_status;

    CStepInfo(const int &_status) : step_status(_status) 
    {
        nowTime = WhatTime();
    }
};

class xCallbackObj 
{
public:
    xCallbackObj() : ccd_flow(0),inter_timeout(0),inter_timeout_able(true),step_i(0)
    {
        mObjMeth = NULL;
        SET_HANDLER(this,&xCallbackObj::TimeOut);
        StepTime();
		flow = FlowNo();
        start_time = time(NULL);
    }

    virtual ~xCallbackObj() {
        delete mObjMeth;

        CStepInfo info(10000);
        stepTimeV.push_back(info);
    }

    void SetCallback(ObjectMethodBase *p) 
    {
        delete mObjMeth;
        mObjMeth = p;
    }

	virtual int TimeOut(int code,void *data) {	return 0;	}
    int HandleEvent(int code,void *data)
    {
        StepTime();
        return mObjMeth->execute(code,data);
    }

    int CostInfo(string &info);
    int CostInfo();
	unsigned flow;
    unsigned ccd_flow;
    time_t access_time;
    time_t start_time;
    time_t inter_timeout;
    bool inter_timeout_able;

    string ObjName;
    string StepName;
    
protected:
    void StepTime() {
        CStepInfo info(step_i++);
        stepTimeV.push_back(info);
        return;
    }
    vector<CStepInfo> stepTimeV;
    unsigned step_i;
private:
    ObjectMethodBase *mObjMeth;
};



/*
    1. ע�� SET_HANDLER
    2. ��ʱ�䴦�����У�����xCObject->HandleEvent �͵���ע��ĺ���������ʲôʱ�򣬶����Ե��� HandleEvent��
       xCObject->HandleEvent �൱�� do_next_step
       ���еĺ���ԭ�Ͷ���
       int HandleEvent(int code,void *data)
       ������� -1 �Ļ����ͱ�ʾ�Ѿ�������ɣ�����ɾ����
*/

#endif

