#ifndef BONES_CORE_THREAD_DISPATCHER_H_
#define BONES_CORE_THREAD_DISPATCHER_H_

#include "event.h"
#include "mutex.h"
#include <queue>
#include <memory>

namespace bones
{

class Root;
class View;

enum ThreadMsgType
{
    kThreadMsgEvent,
    kThreadMsgSetFocus,

    kThreadMsgNeedFocus,
    kThreadMsgCount,
};

struct ThreadMsg
{
    int type;
    RefPtr<Root> target;
};

struct ThreadMsgSetFocus : public ThreadMsg
{
    ThreadMsgSetFocus(Root * r){ type = kThreadMsgSetFocus;  target.reset(r); }
    bool focus;
};

typedef std::shared_ptr<Event> EventPtr;
struct ThreadMsgEvent : public ThreadMsg
{ 
    ThreadMsgEvent(Root * r){ type = kThreadMsgEvent;  target.reset(r); }
    EventPtr e;
};

struct ThreadMsgNeedFocus : public ThreadMsg
{
    ThreadMsgNeedFocus(Root * r){ type = kThreadMsgNeedFocus; target.reset(r); }
    RefPtr<View> who;
};


class ThreadDispatcher
{
public:
    class Action
    {
    public:
        //防止内存泄漏
        virtual ~Action(){};

        virtual void onRun() = 0;
    };
    typedef std::shared_ptr<Action> ActionPtr;

    template <class T>
    static ThreadMsg * Wrap(T & t)
    {
        return new T(t);
    }
public:
    //仅UI线程调用
    void push(ThreadMsg * msg);
    //任意线程调用
    void invoke(ActionPtr action);

    void run();
private:
    bool dispatch();
private:
    std::queue<ThreadMsg *> msg_queue_;
    std::queue<ActionPtr> action_queue_;
    Mutex mutex_;
};

}

#endif