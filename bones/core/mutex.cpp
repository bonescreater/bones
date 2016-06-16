#include "mutex.h"
#include <Windows.h>

namespace bones
{

Mutex::Mutex()
{
    storage_ = new CRITICAL_SECTION;
    ::InitializeCriticalSection(static_cast<LPCRITICAL_SECTION>(storage_));
}

Mutex::~Mutex()
{
    ::DeleteCriticalSection(static_cast<LPCRITICAL_SECTION>(storage_));
    delete storage_;
}

bool Mutex::tryAcquire()
{
    return !!TryEnterCriticalSection(static_cast<LPCRITICAL_SECTION>(storage_));
}

void Mutex::acquire()
{
    ::EnterCriticalSection(static_cast<LPCRITICAL_SECTION>(storage_));
}

void Mutex::release()
{
    ::LeaveCriticalSection(static_cast<LPCRITICAL_SECTION>(storage_));
}


AutoMutex::AutoMutex(Mutex & m)
{
    mutex_ = &m;
    mutex_->acquire();
}

AutoMutex::~AutoMutex()
{
    mutex_->release();
}

}