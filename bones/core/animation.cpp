#include "animation.h"
#include "helper.h"
#include "logging.h"

namespace bones
{

Animation::Animation(View * target, uint64_t interval, uint64_t due, void * ud)
:stopped_(true), paused_(true), start_(false), user_data_(ud),
interval_(interval), due_(due), last_run_(0), running_count_(0)
{
    LOG_VERBOSE << "create:" << "Animation" << this;
    target_.reset(target);
}

Animation::~Animation()
{
    LOG_VERBOSE << "destroy:" << "Animation" << this;
}

void Animation::bind(Action action, const CFRoutine & routine)
{
    if (action >= kCount)
        return;
    action_routine_[action] = routine;
}

void Animation::bind(const CFRun & routine)
{
    run_routine_ = routine;
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
            run_routine_ ? run_routine_(this, target(), 1) : 0;
        onStop();
    } 
}

bool Animation::isStopped() const
{
    return stopped_;
}

void * Animation::userData() const
{
    return user_data_;
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

View * Animation::target() const
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
        if (running_count_ == 0 || -1 == due_)
            progress = 0.f;
        else if (running_count_ == due_)
            progress = 1.0f;
        else
            progress = (float)running_count_ / due_;
        run_routine_ ? run_routine_(this, target(), progress) : 0;
        last_run_ = running_count_;
    }

    if (need_stop)
        stop(false);
}

void Animation::pushAction(Action action)
{
    auto & routine = action_routine_[action];
    routine? routine(this, target()) : 0;
}

}