#include "accelerator_manager.h"

namespace bones
{

AcceleratorManager::AcceleratorManager()
{
    ;
}

AcceleratorManager::~AcceleratorManager()
{
    ;
}

void AcceleratorManager::regAccelerator(const Accelerator & accelerator, AcceleratorTarget * target)
{
    auto & targets = accelerators_[accelerator];
    if (std::find(targets.begin(), targets.end(), target) != targets.end())
        return;
    targets.push_front(target);
}

void AcceleratorManager::unregAccelerator(const Accelerator & accelerator, AcceleratorTarget * target)
{
    auto iter = accelerators_.find(accelerator);
    if (iter == accelerators_.end())
        return;
    iter->second.remove(target);
}

void AcceleratorManager::unregAllAccelerator(AcceleratorTarget * target)
{
    for (auto iter = accelerators_.begin(); iter != accelerators_.end(); ++iter)
        iter->second.remove(target);
}

bool AcceleratorManager::process(const Accelerator & accelerator)
{
    auto iter = accelerators_.find(accelerator);
    if (iter == accelerators_.end())
        return false;

    auto & targets = iter->second;
    for (auto tar_iter = targets.begin(); tar_iter != targets.end(); ++tar_iter)
    {
        if ((*tar_iter)->canProcess() && (*tar_iter)->process(accelerator))
            return true;
    }
    return false;
}




}