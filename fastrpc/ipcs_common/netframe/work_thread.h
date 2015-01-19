// Copyright (c) 2013, Jack.
// All rights reserved.
//
// Author: Jack <lapsangx@gmail.com>
// Created: 05/08/13
// Description: work thread

#ifndef IPCS_COMMON_NETFRAME_WORK_THREAD_H
#define IPCS_COMMON_NETFRAME_WORK_THREAD_H

#include <list>
#include <vector>
#include "command_event.h"
#include "socket_context.h"
#include "mutex.h"
#include "spinlock.h"
#include "base_thread.h"

#if defined _WIN32
#include "wsa_event_poller.h"
#elif defined __linux__
#include "epoll_event_poller.h"
#else
#error not supported platform
#endif

namespace netframe {

class NetFrame;

#if defined _WIN32
typedef WsaEventPoller EventPollerType;
#else
typedef EpollEventPoller EventPollerType;
#endif

class WorkThread: public BaseThread,
    private EventPoller::EventHandler // ������ص�
{
public:
    WorkThread(NetFrame* net_frame, size_t max_fd_value = 0x10000);
    virtual ~WorkThread();

    virtual void Entry();

    /// @brief ����߳����һ�������¼�
    /// @param event ����ӵ������¼�
    void AddCommandEvent(const CommandEvent& event);

private:
    /// ʵ�ֻ���EventHandler�Ĵ��麯���ӿ�
    virtual bool HandleIoEvent(const IoEvent& event);
    virtual bool HandleInterrupt();

private: /// ��������еĴ�����
    /// @brief ������߳��ϵ��¼�
    /// @param event ָ�򱻴�����¼���ָ��
    void ProcessCommandEvent(const CommandEvent& event);

    /// @brief ��ȡ���߳����յ��������¼�
    /// @param event ���ܵ��������¼�����
    /// @retval true �������¼�����
    /// @retval false û�������¼�����
    bool GetCommandEvents(std::list<CommandEvent>& events);

    /// �Ѵ�������¼��ڵ�Ż�freelist�������ռ�������
    void PutCommandEvents(std::list<CommandEvent>& events);

    /// @brief ������е������¼�
    void ClearCommandEvents();

    /// @brief �Ե��������¼���������
    void ClearCommandEvent(const CommandEvent& event);
private:
    typedef Mutex LockType;
    LockType m_Lock;
    NetFrame* m_NetFrame;  ///< ����ʹ�ã����ڻ�ȡworkthread��Ӧ��netframe����
    std::vector<SocketContext*> m_SocketContexts;  ///< �̴߳����SocketContext�б�
    std::list<CommandEvent> m_CommandEventList;    ///< �¼�����
    std::list<CommandEvent> m_CommandEventFreeList; ///< �¼��ڵ㻺�����
    EventPollerType m_EventPoller;
};

} // namespace netframe

#endif // IPCS_COMMON_NETFRAME_WORK_THREAD_H

