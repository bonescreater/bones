#include "res_manager.h"

namespace bones
{

void ResManager::add(const char * key, Pixmap & pm)
{
    if (!key)
        return;
    key2pm_[key] = pm;
}

void ResManager::add(const char * key, Cursor cursor)
{
    if (!key)
        return;
    key2cursor_[key] = cursor;
}

void ResManager::clean()
{
    key2pm_.clear();
    key2cursor_.clear();
}

Pixmap ResManager::getPixmap(const char * key)
{
    if (key)
    {
        auto iter = key2pm_.find(key);
        if (iter != key2pm_.end())
            return iter->second;
    }
    return Pixmap();
}

Cursor ResManager::getCursor(const char * key)
{
    if (key)
    {
        auto iter = key2cursor_.find(key);
        if (iter != key2cursor_.end())
            return iter->second;
    }
    return 0;
}

}