#ifndef BONES_CORE_ANIMATION_H_
#define BONES_CORE_ANIMATION_H_

#include "ref.h"

namespace bones
{

class Animation : public Ref
{
public:
    //progress 0~1
    typedef std::function<void(Ref * sender, Ref * target, float progress, void * user_data)> CFRun;
    typedef std::function<void(Ref * sender, Ref * target, void * user_data)> CFRoutine;

    enum Action
    {
        kStart = 0,
        kStop,
        kPause,
        kResume,
        kCount,
    };

    struct Routine
    {
        CFRoutine func;
        void * user;
    };
public:
    Animation(Ref * target, uint64_t interval, uint64_t due);

    virtual ~Animation();

    void bind(Action action, const CFRoutine & routine, void * user_data);

    void bind(const CFRun & rountine, void * user_data);

    bool isPaused() const;

    bool isRunning() const;

    Ref * target() const;

    bool isStopped() const;
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
    RefPtr<Ref> target_;

    uint64_t interval_;
    uint64_t due_;
    uint64_t last_run_;
    uint64_t running_count_;
    CFRun run_routine_;
    void * run_user_data_;
    Routine action_routine_[kCount];

    friend class AnimationManager;
};


}

#endif