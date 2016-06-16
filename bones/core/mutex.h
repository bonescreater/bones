#ifndef BONES_CORE_MUTEX_H_
#define BONES_CORE_MUTEX_H_

#include "core.h"



namespace bones
{

class Mutex : public NoncopyableObject
{
public:
    Mutex();

    ~Mutex();

    bool tryAcquire();

    void acquire();

    void release();
private:
    void * storage_;
};

class AutoMutex
{
public:
    AutoMutex(Mutex & m);

    ~AutoMutex();
private:
    Mutex * mutex_;
};
}

#endif