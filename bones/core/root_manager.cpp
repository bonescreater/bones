#include "root_manager.h"


namespace bones
{

RootManager::~RootManager()
{
    roots_.clear();
}

Root * RootManager::getRoot(int index) const
{
    if (static_cast<int>(roots_.size()) <= index)
        return nullptr;
    return roots_[index].get();
}

size_t RootManager::getRootCount() const
{
    return roots_.size();
}

void RootManager::add(Root * root)
{
    if (!root)
        return;
    RefPtr<Root> rr;
    rr.reset(root);
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
    {
        if ((*iter) == rr)
            return;
    }
    roots_.push_back(rr);
}

void RootManager::remove(Root * root)
{
    if (!root)
        return;
    RefPtr<Root> rr;
    rr.reset(root);
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
    {
        if ((*iter) == rr)//remove不是直接删除而是置空 防止在迭代过程中remove
            (*iter).reset();
    }
}

void RootManager::remove()
{
    for (auto iter = roots_.begin(); iter != roots_.end(); ++iter)
        (*iter).reset();
}

void RootManager::update()
{
    for (auto iter = roots_.begin(); iter != roots_.end(); )
    {
        if (*iter)
        {
            (*iter)->update();
            iter++;
        }
        else
            iter = roots_.erase(iter);         
    }
}

}