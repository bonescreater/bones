#ifndef BONES_CORE_ACCELERATOR_MANAGER_H_
#define BONES_CORE_ACCELERATOR_MANAGER_H_

#include "accelerator.h"
#include <list>
#include <map>

namespace bones
{

class AcceleratorManager
{
public:
    AcceleratorManager();

    ~AcceleratorManager();

    void regAccelerator(const Accelerator & accelerator, AcceleratorTarget * target);

    void unregAccelerator(const Accelerator & accelerator, AcceleratorTarget * target);

    void unregAllAccelerator(AcceleratorTarget * target);

    bool process(const Accelerator & accelerator);
private:
    typedef std::list<AcceleratorTarget *> AcceleratorTargetList;
    typedef std::map<Accelerator, AcceleratorTargetList> AcceleratorMap;

    AcceleratorMap accelerators_;
};

}

#endif