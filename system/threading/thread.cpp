// Copyright (c) 2012, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>
// Created: 01/13/12
// Description: Thread implementation

#include "toft/system/threading/thread.h"

#include "toft/system/check_error.h"
#include "toft/system/threading/base_thread.h"

#include "thirdparty/glog/logging.h"

namespace toft {

class Thread::Impl : public BaseThread
{
public:
    Impl() {}

    explicit Impl(const std::function<void ()>& function) :
        m_function(function)
    {
    }

    ~Impl()
    {
        m_function = NULL;
    }

    void Initialize(const std::function<void ()>& function)
    {
        CheckNotJoinable();
        m_function = function;
    }

private:
    virtual void Entry()
    {
        m_function();
    }

    virtual void OnExit()
    {
        if (!IsJoinable())
            delete this;
        else
            BaseThread::OnExit();
    }

    void CheckNotJoinable() const
    {
        // Can't reinitialze a joinable thread
        if (IsJoinable())
            TOFT_CHECK_ERRNO_ERROR(EINVAL);
    }

private:
    // Description about the routine by Function.
    // If it's set, ignore the above routine.
    std::function<void ()> m_function;
};


Thread::Thread(): m_pimpl(new Impl())
{
}

Thread::Thread(const std::function<void ()>& function) :
    m_pimpl(new Impl(function))
{
    m_pimpl->Start();
}

Thread::~Thread()
{
    delete m_pimpl;
    m_pimpl = NULL;
}

void Thread::SetStackSize(size_t size)
{
    return m_pimpl->SetStackSize(size);
}

void Thread::Start(const std::function<void ()>& function)
{
    m_pimpl->Initialize(function);
    m_pimpl->Start();
}

bool Thread::TryStart(const std::function<void ()>& function)
{
    m_pimpl->Initialize(function);
    return m_pimpl->TryStart();
}

bool Thread::Join()
{
    return m_pimpl->Join();
}

void Thread::Detach()
{
    // After detached, the m_pimpl will be deleted in Thread::Impl::OnExit.
    // So don't delete it here, just set it be NULL to mark this thread object
    // to be detached.
    m_pimpl->DoDetach();
    m_pimpl = NULL;
}

void Thread::SendStopRequest()
{
    m_pimpl->SendStopRequest();
}

bool Thread::IsStopRequested() const
{
    return m_pimpl->IsStopRequested();
}

bool Thread::StopAndWaitForExit()
{
    return m_pimpl->StopAndWaitForExit();
}

bool Thread::IsAlive() const
{
    return m_pimpl && m_pimpl->IsAlive();
}

bool Thread::IsJoinable() const
{
    return m_pimpl && m_pimpl->IsJoinable();
}

ThreadHandleType Thread::GetHandle() const
{
    return m_pimpl->GetHandle();
}

int Thread::GetId() const
{
    return m_pimpl->GetId();
}

} // namespace toft
