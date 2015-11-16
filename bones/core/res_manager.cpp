#include "res_manager.h"

namespace bones
{

void ResManager::addPixmap(const char * key, Pixmap & pm)
{
    if (!key)
        return;
    key2pm_[key] = pm;
}


void ResManager::clean()
{
    key2pm_.clear();
}

Pixmap * ResManager::getPixmap(const char * key)
{
    if (key)
    {
        auto iter = key2pm_.find(key);
        if (iter != key2pm_.end())
            return &iter->second;
    }
    return nullptr;
}

}