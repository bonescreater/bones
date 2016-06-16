#include "thread_dispatcher.h"
#include "root.h"

namespace bones
{

void ThreadDispatcher::push(ThreadMsg * msg)
{
    msg_queue_.push(msg);
}

void ThreadDispatcher::invoke(ActionPtr action)
{
    if (nullptr == action)
        return;
    AutoMutex am(mutex_);
    action_queue_.push(action);
}

void ThreadDispatcher::run()
{
    //有事件仅处理事件
    if (dispatch())
        return;
    //处理action
    mutex_.acquire();
    while (!action_queue_.empty())
    {
        auto front = action_queue_.front();
        action_queue_.pop();
        if (nullptr == front)
            continue;
        mutex_.release();
        front->onRun();
        mutex_.acquire();

    }
    mutex_.release();
}

bool ThreadDispatcher::dispatch()
{
    bool empty = msg_queue_.empty();
    while (!msg_queue_.empty())
    {
        auto front = msg_queue_.front();
        msg_queue_.pop();
        assert(nullptr != front->target);
        assert(this == &(front->target->getContext().getDispatcher()));

        switch (front->type)
        {
        case kThreadMsgEvent:
        {
            auto msg = static_cast<ThreadMsgEvent *>(front);
            msg->target->handMsg(msg->e.get());
            delete msg;
        }
            break;
        case kThreadMsgSetFocus:
        {
            auto msg = static_cast<ThreadMsgSetFocus *>(front);
            msg->target->handMsg(msg->focus);
        }
            break;
        case kThreadMsgNeedFocus:
        {
             auto msg = static_cast<ThreadMsgNeedFocus *>(front);
             msg->target->handMsg(msg->who.get());
             delete msg;
        }
            break;
        default:
            assert(0);
        }
    }
    return !empty;
}

}