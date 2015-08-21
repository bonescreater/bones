#include "animation.h"
#include "helper.h"
#include "logging.h"

namespace bones
{

Animation::Animation(Ref * target, uint64_t interval, uint64_t due)
:stopped_(true), paused_(true), start_(false),
interval_(interval), due_(due), last_run_(0), running_count_(0),
run_user_data_(nullptr)
{
    LOG_VERBOSE << "create:" << "Animation" << this;
    target_.reset(target);
    for (auto i = 0; i < kCount; i++)
        action_routine_[i].user = nullptr;
}

Animation::~Animation()
{
    LOG_VERBOSE << "destroy:" << "Animation" << this;
}

void Animation::bind(Action action, const CFRoutine & routine, void * user_data)
{
    if (action >= kCount)
        return;
    action_routine_[action].func = routine;
    action_routine_[action].user = user_data;
}

void Animation::bind(const CFRun & routine, void * user_data)
{
    run_routine_ = routine;
    run_user_data_ = user_data;
}
//避免重复start  每个Animation只能start 和 stop 一次
void Animation::start()
{
    if (!start_)
    {
        start_ = true;
        stopped_ = false;
        paused_ = false;
        onStart();
    }
}

void Animation::stop(bool end)
{
    if (!isStopped())
    {
        stopped_ = true;
        if (end)
            run_routine_ ? run_routine_(this, target(), 1, run_user_data_) : 0;
        onStop();
    } 
}

bool Animation::isStopped() const
{
    return stopped_;
}

void Animation::pause()
{
    if (isRunning())
    {
        paused_ = true;
        onPause();
    }
    
}

void Animation::resume()
{
    if (!isStopped() && isPaused())
    {
        paused_ = false;
        onResume();
    }    
}

bool Animation::isPaused() const
{
    return paused_;
}

bool Animation::isRunning() const
{
    return !isStopped() && !isPaused();
}

Ref * Animation::target() const
{
    return target_.get();
}

void Animation::run(uint64_t delta)
{
    if (isRunning())
        onRun(delta);
}

void Animation::onStart()
{
    pushAction(kStart);
}

void Animation::onStop()
{
    pushAction(kStop);
}

void Animation::onPause()
{
    pushAction(kPause);
}

void Animation::onResume()
{
    pushAction(kResume);
}

void Animation::onRun(uint64_t delta)
{
    running_count_ += delta;

    bool notify = false;
    bool need_stop = false;

    if (running_count_ - last_run_ >= interval_)
        notify = true;

    if (-1 != due_ && running_count_ >= due_)
    {//总时间有效 而且 运行时间已经超过总时间了
        running_count_ = due_;
        need_stop = true;
        notify = true;
    }

    if (notify)
    {
        float progress = 0;
        if (running_count_ == due_)
            progress = 1.0f;
        else if (running_count_ == 0)
            progress = 0.f;
        else
            progress = (float)running_count_ / due_;
        run_routine_ ? run_routine_(this, target(), progress, run_user_data_) : 0;
        last_run_ = running_count_;
    }

    if (need_stop)
        stop(false);
}

void Animation::pushAction(Action action)
{
    auto & routine = action_routine_[action];
    routine.func ? routine.func(this, target(), routine.user) : 0;
}

}