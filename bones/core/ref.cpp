
#include "ref.h"

#include <intrin.h>
namespace bones
{

Ref::Ref() :count_(1)
{    
    ;
}

Ref::~Ref()
{
    assert(count_ == 0);
}

void Ref::retain()
{
    assert(count_ > 0);
    ::_InterlockedIncrement(&count_);
}

void Ref::release()
{
    assert(count_ > 0);
    ::_InterlockedDecrement(&count_);
    if (0 == count_)       
        delete this;      
}

int Ref::getRefCount() const
{
    return count_;
}

NoncopyableObject::NoncopyableObject()
{

}

}