#include "res_manager.h"

namespace bones
{

void ResManager::addPixmap(const char * key, Pixmap & pm)
{
    if (!key)
        return;
    key2pm_[key] = pm;
}

void ResManager::addCursor(const char * key, Cursor cursor)
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

Cursor ResManager::getCursor(const char * key)
{
    if (key)
    {
        auto iter = key2cursor_.find(key);
        if (iter != key2cursor_.end())
            return iter->second;

        const wchar_t * cursor = nullptr;
        if (!strcmp(key, "arrow"))
            cursor = IDC_ARROW;
        else if (!strcmp(key, "ibeam"))
            cursor = IDC_IBEAM;
        else if (!strcmp(key, "wait"))
            cursor = IDC_WAIT;
        else if (!strcmp(key, "cross"))
            cursor = IDC_CROSS;
        else if (!strcmp(key, "up-arrow"))
            cursor = IDC_UPARROW;
        else if (!strcmp(key, "size"))
            cursor = IDC_SIZE;
        else if (!strcmp(key, "icon"))
            cursor = IDC_ICON;
        else if (!strcmp(key, "size-nwse"))
            cursor = IDC_SIZENWSE;
        else if (!strcmp(key, "size-nesw"))
            cursor = IDC_SIZENESW;
        else if (!strcmp(key, "size-we"))
            cursor = IDC_SIZEWE;
        else if (!strcmp(key, "size-ns"))
            cursor = IDC_SIZENS;
        else if (!strcmp(key, "size-all"))
            cursor = IDC_SIZEALL;
        else if (!strcmp(key, "no"))
            cursor = IDC_NO;
        else if (!strcmp(key, "hand"))
            cursor = IDC_HAND;
        else if (!strcmp(key, "app-starting"))
            cursor = IDC_APPSTARTING;
        else if (!strcmp(key, "help"))
            cursor = IDC_HELP;
        if (cursor)
            return ::LoadImage(NULL, cursor, IMAGE_CURSOR, 0, 0, LR_SHARED);
    }
    return 0;
}

}