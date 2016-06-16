#ifndef BONES_CORE_ATOMIC_H_
#define BONES_CORE_ATOMIC_H_

#include "core.h"

namespace bones
{

class Atomic
{
public:
    Atomic();

    ~Atomic();

    int operator=(int value);
    int operator+=(int addend);
    int operator-=(int addend);
    int operator++();
    int operator--();
    operator int() const;
private:
    volatile long value_;
};

}

#endif