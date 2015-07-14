#include "animation.h"
#include "helper.h"

namespace bones
{

Animation::Animation()
:running_count_(0), stopped_(true), paused_(true)
{
    ;
}

Animation::~Animation()
{
    ;
}

void Animation::start()
{
    running_count_ = 0;
    stopped_ = false;
    paused_ = false;
}

void Animation::stop()
{
    stopped_ = true;
}

bool Animation::isStopped() const
{
    return stopped_;
}

void Animation::pause()
{
    paused_ = true;
}

void Animation::resume()
{
    paused_ = false;
}

bool Animation::isPaused() const
{
    return paused_;
}

void Animation::run(uint64_t delta)
{
    if (isStopped() || isPaused())
        return;

    running_count_ += delta;
    onRun(running_count_);
}

const char * Animation::getClassName() const
{
    return kClassAnimation;
}

void Animation::onRun(uint64_t running)
{
    ;
}

}