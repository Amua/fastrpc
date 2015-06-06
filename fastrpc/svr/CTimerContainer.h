#ifndef _STORAGE_CONTAINER_TIMER_H_
#define _STORAGE_CONTAINER_TIMER_H_

#include <sys/time.h>
#include <map>
#include "CCallbackObj.h"

using namespace std;


/**
* @author feimat@baidu.com
*
* 
* <pre>
* ���ģ�ͣ��첽�ص���״̬���Ĺ�����
* </pre>
**/




class CCallbackObjQueue
{
public:
	CCallbackObjQueue(){};
	virtual ~CCallbackObjQueue(){};
	//
	// ��װtimer
	// return 0		�ɹ�
	//	      <0	ʧ��
	//
	int Set(const unsigned &flow,xCallbackObj *obj,time_t __mIntervalSecond = 120 /* ��λ�� */);
	
	//
	// ���timer��Ӧ�����ݣ�����ж��timer
	// return 0		�ɹ�
	//        <0    ������
	//
	int Get(const unsigned &flow,xCallbackObj** obj);

    unsigned Size() {
        return objQueue.size();
    }

	void CheckTimeout();

protected:
	map<unsigned,xCallbackObj*> objQueue;
};




#endif
