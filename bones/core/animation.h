#ifndef BONES_CORE_ANIMATION_H_
#define BONES_CORE_ANIMATION_H_

#include "ref.h"

namespace bones
{

class Animation : public Ref
{
public:
    void start();

    void stop();

    bool isStopped() const;

    void pause();

    void resume();

    bool isPaused() const;

    bool isRunning() const;

    void run(uint64_t delta);

    virtual const char * getClassName() const override;
protected:
    Animation();

    virtual ~Animation();

    virtual void onStart();

    virtual void onStop();

    virtual void onPause();

    virtual void onResume();

    virtual void onRun(uint64_t due_running);
private:
    uint64_t running_count_;
    bool stopped_;
    bool paused_;
    bool start_;
};

class GeneralAnimation : public Animation
{
public:
    typedef std::function<void(Ref & sender, uint64_t due_running)> CFRoutine;
public:
    GeneralAnimation(uint64_t interval, uint64_t due, const CFRoutine & routine);

    virtual ~GeneralAnimation();

    void onRun(uint64_t due_running) override;
private:
    uint64_t interval_;
    uint64_t due_;
    uint64_t last_run_;
    CFRoutine routine_;
};


}

#endif