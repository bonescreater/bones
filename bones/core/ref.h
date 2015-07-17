#ifndef BONES_CORE_REF_H_
#define BONES_CORE_REF_H_

#include "core.h"

namespace bones
{

class NoncopyableObject
{
protected:
    NoncopyableObject();
private:
    NoncopyableObject(const NoncopyableObject&);
    NoncopyableObject& operator=(const NoncopyableObject&);
};

class Ref : public NoncopyableObject
{
public:
    Ref();

    virtual ~Ref();

    void retain();

    void release();

    int getRefCount() const;

    virtual const char * getClassName() const = 0;
private:
    mutable int count_;
};

template<typename T>
class RefPtr 
{
public:
    RefPtr() : ptr_(nullptr) {}

    RefPtr(const RefPtr & other)
        : ptr_(other.get()) 
    {
        if (ptr_)
            ptr_->retain();
    }
    

    template<typename U>
    RefPtr(const RefPtr<U>& other)
        : ptr_(other.get()) 
    {
        if (ptr_)
            ptr_->retain();
    }

    ~RefPtr() 
    {
        clear();
    }

    RefPtr & operator = (const RefPtr & other) 
    {
        if (this->ptr_)
            this->ptr_->release();
        if (other.get())
            other.get()->retain();
        this->ptr_ = other.get();

        return *this;
    }

    template<typename U>
    RefPtr & operator= (const RefPtr<U> & other) 
    {
        if (this->ptr_)
            this->ptr_->release();
        if (other.get())
            other.get()->retain();
        this->ptr_ = other.get();

        return *this;
    }

    bool operator< (const RefPtr & right) const
    {
        return ptr_ < right.get();
    }

    template<typename U>
    bool operator < (const RefPtr<U> & right) const
    {
        return ptr_ < right.get();
    }

    bool operator == (const RefPtr & right) const
    {
        return ptr_ == right.get();
    }

    bool operator != (const RefPtr & right) const
    {
        return !(*this == right);
    }

    template<typename U>
    bool operator == (const RefPtr<U> & right) const
    {
        return ptr_ == right.get();
    }

    void reset()
    {
        if (ptr_)
            ptr_->release();
        ptr_ = nullptr;
    }

    void reset(T * ptr)
    {
        if (ptr_)
            ptr_->release();
        ptr_ = ptr;
        if (ptr_)
            ptr_->retain();

    }

    void clear() 
    {
        T * to_unref = ptr_;
        ptr_ = nullptr;
        if (to_unref)
            to_unref->release();
    }

    T * get() const { return ptr_; }

    T & operator*() const { return *ptr_; }

    T * operator->() const { return ptr_; }

    typedef T* RefPtr::*unspecified_bool_type;
    operator unspecified_bool_type() const 
    {
        return ptr_ ? &RefPtr::ptr_ : nullptr;
    }

    
private:
    T * ptr_;

    explicit RefPtr(T* ptr) :ptr_(ptr){}

    template<typename U>
    friend RefPtr<U> AdoptRef(U* ptr);
};

template<typename T>
RefPtr<T> AdoptRef(T* ptr) 
{ 
    return RefPtr<T>(ptr);
}


}


#endif