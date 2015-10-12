#ifndef BONES_CORE_ANIMATION_H_
#define BONES_CORE_ANIMATION_H_

#include "view.h"
#include <functional>

namespace bones
{

class Animation : public Ref
{
public:
    //progress 0~1
    typedef std::function<void(Animation * sender, View * target, float progress)> CFRun;
    typedef std::function<void(Animation * sender, View * target)> CFRoutine;

    enum Action
    {
        kStart = 0,
        kStop,
        kPause,
        kResume,
        kCount,
    };

public:
    Animation(View * target, uint64_t interval, uint64_t due, void * ud = nullptr);

    virtual ~Animation();

    void bind(Action action, const CFRoutine & routine);

    void bind(const CFRun & rountine);

    bool isPaused() const;

    bool isRunning() const;

    View * target() const;

    bool isStopped() const;

    void * userData() const;
protected:
    void start();

    void stop(bool end);

    void pause();

    void resume();

    void run(uint64_t delta);

    virtual void onStart();

    virtual void onStop();

    virtual void onPause();

    virtual void onResume();

    virtual void onRun(uint64_t due_running);
private:
    void pushAction(Action action);
private:
    bool stopped_;
    bool paused_;
    bool start_;
    RefPtr<View> target_;
    void * user_data_;

    uint64_t interval_;
    uint64_t due_;
    uint64_t last_run_;
    uint64_t running_count_;
    CFRun run_routine_;
    CFRoutine action_routine_[kCount];

    friend class AnimationManager;
};


}

#endif