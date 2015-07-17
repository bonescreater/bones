#ifndef BONES_CORE_ANIMATION_H_
#define BONES_CORE_ANIMATION_H_

#include "ref.h"

namespace bones
{

class Animation : public Ref
{
protected:
    Animation(Ref * target);

    virtual ~Animation();

    void start();

    void stop();

    bool isStopped() const;

    void pause();

    void resume();

    bool isPaused() const;

    bool isRunning() const;

    Ref * target() const;

    void run(uint64_t delta);

    virtual void onStart();

    virtual void onStop();

    virtual void onPause();

    virtual void onResume();

    virtual void onRun(uint64_t due_running);
private:
    bool stopped_;
    bool paused_;
    bool start_;
    RefPtr<Ref> target_;
    friend class AnimationManager;
};

class CommonAnimation : public Animation
{
public:
    typedef std::function<void(Ref * sender, Ref * target, uint64_t due_running, void * user_data)> CFRun;
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
    CommonAnimation(Ref * target, uint64_t interval, uint64_t due);

    virtual ~CommonAnimation();

    void bind(Action action, const CFRoutine & routine, void * user_data = nullptr);

    void bind(const CFRun & rountine, void * user_data = nullptr);

    void onStart() override;

    void onStop() override;

    void onPause() override;

    void onResume() override;

    void onRun(uint64_t delta) override;

    const char * getClassName() const override;
private:
    void pushAction(Action action);
private:
    uint64_t interval_;
    uint64_t due_;
    uint64_t last_run_;
    uint64_t running_count_;
    CFRun run_routine_;
    void * run_user_data_;
    Routine action_routine_[kCount];
};


}

#endif