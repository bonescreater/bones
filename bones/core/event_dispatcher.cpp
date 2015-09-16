
#include "event_dispatcher.h"
#include "event.h"
#include "view.h"

namespace bones
{

void EventDispatcher::Push(Event & e)
{
    EventDispatcher dispatcher;
    EventDispatcher::Path path;
    EventDispatcher::GetPath(e.target(), path);
    dispatcher.run(e, path);
}

void EventDispatcher::GetPath(View * v, Path & path)
{
    path.clear();

    View * node = v;
    while (node)
    {
        RefPtr<View> tmp;
        tmp.reset(node);
        path.push_back(tmp);
        node = node->parent();
    }
}

void EventDispatcher::run(Event & e, Path & path)
{
    e.attach(&path);
    View * target = e.target();
    if (!target)
        return;

    //capture阶段
    e.phase_ = Event::kCapture;
    for (auto item = path.rbegin(); item != path.rend(); ++item)
    {
        View * current = item->get();
        if (current != target)
        {
            current->dispatch(e);
            if (!e.propagation_)
                return;
        }

    }

    //target
    e.phase_ = Event::kTarget;
    target->dispatch(e);
    if (!e.propagation_)
        return;

    //不可以冒泡
    if (!e.bubbles_)
        return;

    //冒泡阶段
    e.phase_ = Event::kBubbling;
    for (auto item = path.begin(); item != path.end(); ++item)
    {
        View * current = item->get();
        if (current != target)
        {
            current->dispatch(e);
            if (!e.propagation_)
                return;
        }
    }
    return;
}







}