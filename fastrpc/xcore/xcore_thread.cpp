// 2008-02-23
// xcore_thread.cpp
//
// �߳���ʵ��


#include "xcore_thread.h"
#ifdef __WINDOWS__
#include <process.h>
#endif//__WINDOWS__

namespace xcore {

///////////////////////////////////////////////////////////////////////////////
// class XThread
///////////////////////////////////////////////////////////////////////////////
XThread::XThread()
	: m_id(0)
	, m_state(CREATE)
	, m_runnable(NULL)
	, m_handle(INVALID_HANDLE_VALUE)
	, m_quit(true)
{
	// empty
}

XThread::~XThread()
{
	ASSERT(!m_id && "Need to call join() before XThread destruction.");
}

#ifdef __WINDOWS__
unsigned __stdcall invok_proc_win(void *arg)
{
	static XAtomic32 seed(0);
	::srand((uint32)time(NULL) | ((uint32)(++seed) << 16));

	XThread *pThis = static_cast<XThread *>(arg);
	ASSERT(pThis);
	ASSERT(pThis->m_runnable);
	pThis->m_state = XThread::RUNNING;
	try
	{
	pThis->m_runnable->run(pThis);
	}
	catch(...)
	{
		ASSERT(!"XThread::run() throw exception.");
	}
	pThis->m_state = XThread::STOP;
	return 0;
}

void* invok_proc_posix(void *pParam)
{
	return NULL;
}

bool XThread::start(XRunnable* r, uint32 stack_size)
{
	ASSERT(m_id == 0 && "Thread already start.");
	if (m_id != 0) return true;
	if (r == NULL)
		m_runnable = this;
	else
		m_runnable = r;

	unsigned id = 0;
	m_state = START;
	m_handle = (HANDLE)_beginthreadex(NULL, stack_size, invok_proc_win, this, 0, &id);
	if (m_handle == (HANDLE)-1L)
	{
		m_state = JOINED;
		return false;  // msdn: _beginthread returns -1L on an error.
	}
	m_id = (uint64)id;
	return true;
}

void XThread::stop()
{
	m_quit.set();
}

void XThread::join()
{
	if (m_id == 0) return;
	ASSERT(m_id != xcore::thread_id() && "Cannot join self.");
	WaitForSingleObject(m_handle, INFINITE);
	CloseHandle(m_handle);
	m_handle = INVALID_HANDLE_VALUE;
	m_id = 0;
	m_quit.reset();
	m_state = CREATE;
	return;
}

void XThread::kill()
{
	ASSERT(m_id != xcore::thread_id() && "Cannot kill self.");
	TerminateThread(m_handle, 0);
	m_state = STOP;
}

bool XThread::wait_quit(uint32 msec)
{
	return m_quit.trywait(msec);
}

uint32 thread_id()
{
	return (uint32)GetCurrentThreadId();
}

void sleep(uint32 msec)
{
	// windows����select()��ʵ�ֵ�sleep���ܲ�����(��ʵ��)
	// windows����WaitForSingleObject()ʵ�ֵ�sleep���ܣ�������::Sleep()��ͬ
	// windows��::Sleep(0)�ᱻ����
	// windows��::Sleep()��׼ȷ����ʱ��Ƭ�йأ���ȷ��һ��Ϊ10ms
	if (msec == 0) msec = 1;
	::Sleep(msec);
}
#endif//__WINDOWS__


///////////////////////////////////////////////////////////////////////////////
// class XThread
///////////////////////////////////////////////////////////////////////////////
#ifdef __GNUC__
unsigned __stdcall invok_proc_win(void *arg)
{
	return 0;
}

void* invok_proc_posix(void *arg)
{
	static XAtomic32 seed(0);
	::srand((uint32)time(NULL) | ((uint32)(++seed) << 16));

	XThread *pThis = static_cast<XThread *>(arg);
	ASSERT(pThis);
	ASSERT(pThis->m_runnable);
	pThis->m_state = XThread::RUNNING;
	try
	{
	pThis->m_runnable->run(pThis);
	}
	catch(...)
	{
		ASSERT(!"XThread::run() throw exception.");
	}
	pThis->m_state = XThread::STOP;
	return NULL;
}

bool XThread::start(XRunnable* r, uint32 stack_size)
{
	ASSERT(m_id == 0 && "Thread already start.");
	if (m_id != 0) return true;
	if (r == NULL)
		m_runnable = this;
	else
		m_runnable = r;

	pthread_t id = 0;
	int ret = 0;
	m_state = START;
	if (stack_size > 0)
	{
		pthread_attr_t attr;
		if(stack_size < PTHREAD_STACK_MIN) stack_size = PTHREAD_STACK_MIN;
		VERIFY(!pthread_attr_init(&attr));
		VERIFY(!pthread_attr_setstacksize(&attr, stack_size));
		ret = pthread_create(&id, &attr, invok_proc_posix, this);
		VERIFY(!pthread_attr_destroy(&attr));
	}
	else
	{
		ret = pthread_create(&id, NULL, invok_proc_posix, this);
	}
	if (0 != ret)
	{
		m_state = JOINED;
		return false;
	}
	m_id = (uint64)id;
	return true;
}

void XThread::stop()
{
	m_quit.set();
}

void XThread::join()
{
	if (m_id == 0) return;
	ASSERT(m_id != xcore::thread_id() && "Cannot join self.");
	pthread_join(m_id, NULL);
	m_id = 0;
	m_quit.reset();
	m_state = CREATE;
	return;
}

void XThread::kill()
{
	ASSERT(m_id != xcore::thread_id() && "Cannot kill self.");
	pthread_cancel((pthread_t)m_id);
	m_state = STOP;
}

bool XThread::wait_quit(uint32 msec)
{
	return m_quit.trywait(msec);
}

uint32 thread_id()
{
	return (uint32)pthread_self();
}

void sleep(uint32 msec)
{
	// linux�ϵ�nanosleep��alarm��ͬ�����ǻ����ں�ʱ�ӻ���ʵ�֣���linux�ں�ʱ��ʵ�ֵ�Ӱ��
	// Linux/i386����10 ms ��Linux/Alpha����1ms
	// nanosleep();
	// clock_nanosleep();

	// msecΪ0�ᱻ����
	// linux��select()����Ϊ1ms
	if (msec == 0) msec = 1;

	struct timeval tv;
	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec % 1000) * 1000;
	select(0, NULL, NULL, NULL, &tv);
	return;
}

#endif//__GNUC__

} // namespace xcore



////////////////////////////////////////////////////////////////////////////////
// test section
////////////////////////////////////////////////////////////////////////////////

#ifdef _XCORE_NEED_TEST

#include "xcore_test.h"

namespace xcore
{

class MyRunnable : public XRunnable
{
public:
	MyRunnable() {}

	virtual ~MyRunnable() {}

	virtual void run(XThread* pThread)
	{
		ASSERT(pThread);
		printf("MyThread(id:%u) start....\n", pThread->id());
		for (int i = 0; i < 5 && !pThread->wait_quit(0); i++)
		{
			printf("MyRunnable is %d.\n", i);
			xcore::sleep(1000);
		}
		printf("MyThread(id:%u) stop...\n", pThread->id());
		return;
	}
};

bool xcore_test_thread()
{
	XThread thread_;
	MyRunnable runnable_;
	VERIFY(thread_.start(&runnable_));
	xcore::sleep(3000);
	thread_.stop();
	thread_.join();

	VERIFY(thread_.start(&runnable_));
	xcore::sleep(2000);
	thread_.kill();
	thread_.join();

	return true;
}

}//namespace xcore

#endif//_XCORE_NEED_TEST
