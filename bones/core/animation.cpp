#include "animation.h"
#include "helper.h"

namespace bones
{

Animation::Animation()
:running_count_(0), stopped_(true), paused_(true), start_(false)
{
    ;
}

Animation::~Animation()
{
    ;
}
//避免重复start  每个Animation只能start 和 stop 一次
void Animation::start()
{
    if (!start_)
    {
        start_ = true;
        running_count_ = 0;
        stopped_ = false;
        paused_ = false;
        onStart();
    }
}

void Animation::stop()
{
    if (!isStopped())
    {
        stopped_ = true;
        onStop();
    } 
}

bool Animation::isStopped() const
{
    return stopped_;
}

void Animation::pause()
{
    if (!isPaused())
    {
        paused_ = true;
        onPause();
    }
    
}

void Animation::resume()
{
    if (isPaused())
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

void Animation::run(uint64_t delta)
{
    if (isRunning())
    {
        running_count_ += delta;
        onRun(running_count_);
    }
}

const char * Animation::getClassName() const
{
    return kClassAnimation;
}

void Animation::onStart()
{
    ;
}

void Animation::onStop()
{
    ;
}

void Animation::onPause()
{
    ;
}

void Animation::onResume()
{
    ;
}

void Animation::onRun(uint64_t running)
{
    ;
}

GeneralAnimation::GeneralAnimation(uint64_t interval, uint64_t due, const CFRoutine & routine)
:interval_(interval), due_(due), routine_(routine), last_run_(0)
{
    ;
}

GeneralAnimation::~GeneralAnimation()
{
    ;
}

void GeneralAnimation::onRun(uint64_t due_running)
{
    if (0 == last_run_)
    {//定时器首次触发
        last_run_ = due_running;
        return;
    }
    
    bool notify = false;
    bool need_stop = false;

    if (due_running - last_run_ >= interval_)
        notify = true;

    if (-1 != due_ && due_running >= due_)
    {//总时间有效 而且 运行时间已经超过总时间了
        due_running = due_;
        need_stop = true;
        notify = true;
    }

    if (notify)
    {
        if (routine_)
            routine_(*this, due_running);
        last_run_ = due_running;
    }

    if (need_stop)
        stop();
}

}