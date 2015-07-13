#ifndef BONES_CORE_EVENT_DISPATCHER_H_
#define BONES_CORE_EVENT_DISPATCHER_H_

#include "ref.h"
#include <list>


namespace bones
{

class Event;
class View;

class EventDispatcher
{
public:
    typedef std::list<RefPtr<View>> Path;
public:
    static void getPath(View * v, Path & path);
public:
    virtual void run(Event & e, Path & path);
};


}
#endif