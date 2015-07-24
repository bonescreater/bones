#include "panel_manager.h"


namespace bones
{

void PanelManager::add(Panel * panel)
{
    if (!panel)
        return;
    RefPtr<Panel> rp;
    rp.reset(panel);
    auto iter = std::find(panels_.begin(), panels_.end(), rp);
    if (iter != panels_.end())
        return;
    panels_.push_back(rp);
}

void PanelManager::remove(Panel * panel)
{
    if (!panel)
        return;
    RefPtr<Panel> rp;
    rp.reset(panel);
    auto iter = std::find(panels_.begin(), panels_.end(), rp);
    if (iter != panels_.end())
        (*iter).reset();
}

void PanelManager::update()
{
    for (auto iter = panels_.begin(); iter != panels_.end();)
    {
        auto & p = *iter;
        if (p)
        {
            p->update();
            ++iter;
        }
        else
            iter = panels_.erase(iter);
    }
}

Panel * PanelManager::begin()
{
    iter_ = panels_.begin();
    while (iter_ != panels_.end())
    {
        if (*iter_)
            return (*iter_).get();
        else
            iter_ = panels_.erase(iter_);
    }
        
    return nullptr;
}

Panel * PanelManager::next()
{
    if (panels_.end() == iter_)
        return nullptr;
    ++iter_;
    while (iter_ != panels_.end())
    {
        if (*iter_)
            return (*iter_).get();
        else
            iter_ = panels_.erase(iter_);
    }

    return nullptr;
}

}